/*
 * LeveldbWriter.cpp
 *
 *  Created on: 2015-04-02
 *      Author: Moustafa S. Ibrahim
 */

#include <iostream>

#include "leveldb-writer.h"
using std::cerr;
using std::cout;

#include "utilities.h"

const int WRITING_LIMIT = 1000;

namespace MostCV {

LeveldbWriter::LeveldbWriter(string db_path_, int resize_height_, int resize_width_, int volumeSize, bool is_virtual_) {
  max_label_cnt = -1;
  db_path = db_path_;
  resize_height = resize_height_;
  resize_width = resize_width_;
  volume_size = volumeSize;
  is_virtual = is_virtual_;

  cerr<<"\n\nCreates a database at: "<<db_path_<<"\n";

  if(is_virtual_)
    cerr<<"\tUing VIRTUAL MODE dataset\n\n";

  countId = 0;
  lastCountId = 0;
  internal_idx = 1;

  if (resize_height > 0) {  // then something already defined for the shape
    datum.set_channels(volume_size);
    datum.set_height(resize_height);
    datum.set_width(resize_width);

    cerr<<"\t(H, W, C) = "<<resize_height<<" "<<resize_width<<" "<<volume_size<<"\n";
  }
  if(!is_virtual) {
    // leveldb
    leveldb::Options options;
    options.error_if_exists = true;
    options.create_if_missing = true;
    options.write_buffer_size = 268435456;	// 8 * 32 * 1024 * 1024

    // Open db
    LOG(INFO)<< "Opening leveldb " << db_path;
    leveldb::Status status = leveldb::DB::Open(options, db_path, &db);
    CHECK(status.ok()) << "Failed to open leveldb " << db_path << ". Is it already existing?";
    batch = new leveldb::WriteBatch();
  }

  is_closed = false;
}

LeveldbWriter::~LeveldbWriter() {
  forceFinalize();
}

void LeveldbWriter::clearDatum() {
  assert(!is_closed);

  datum.clear_data();
  datum.clear_float_data();
}

void LeveldbWriter::setLabelsRange(int max_label_cnt) {
  assert(!is_closed);

  this->max_label_cnt = max_label_cnt;
}

void LeveldbWriter::setDatumLabel(int id) {
  assert(!is_closed);

  assert(id >= 0);

  if (max_label_cnt != -1 && id >= max_label_cnt) {
    cerr << "Wrong label! (Received, expected) = " << id << " - " << max_label_cnt << "\n";
    assert(false);
  }

  datum.set_label(id);
  labels.insert(id);
  labelsVec.push_back(id);
}

void LeveldbWriter::addDatumToBatch(string key) {
  assert(!is_closed);

  if (key != "" && keys.insert(key).second == false)
    cerr << "Warning: key duplication: " << key << "\n";

  if(is_virtual)  return;

  string value;
  assert(datum.SerializeToString(&value));

  string prefix = MostCV::toIntStr("0000000", internal_idx++) + "@";
  batch->Put(prefix + key, value);

  if (++countId % WRITING_LIMIT == 0)
    writeBatch();

  clearDatum();
}

void LeveldbWriter::addDatumToBatch(caffe::Datum &datum, string key, int label) {
  assert(!is_closed);

  if (keys.insert(key).second == false)
    cerr << "Warning: Key duplication: " << key << "\n";

  assert(label >= 0);

  string value;
  datum.set_label(label);
  labels.insert(label);
  labelsVec.push_back(label);

  if(is_virtual)  return;

  assert(datum.SerializeToString(&value));

  string prefix = MostCV::toIntStr("0000000", internal_idx++) + "@";
  batch->Put(prefix + key, value);

  if (++countId % WRITING_LIMIT == 0)
    writeBatch();

  clearDatum();
}

bool LeveldbWriter::addVectorDatum(const vector<double> &feature_vec) {
  assert(!is_closed);

  if(is_virtual)  return true;

  clearDatum();

  if (resize_height <= 0) {  // use first vector to define the outline
    datum.set_height(resize_height = feature_vec.size());
    datum.set_channels(1);
    datum.set_width(1);
  } else
    assert((int )feature_vec.size() == resize_height * resize_width * volume_size);

  for (int p = 0; p < (int) feature_vec.size(); ++p)
    datum.add_float_data(feature_vec[p]);

  return true;
}

bool LeveldbWriter::addImageToDatum(Mat imgMat_origin, int num_channels) {
  assert(!is_closed);

  if(is_virtual)  return true;

  assert(resize_width > 0 && resize_height > 0);
  assert(imgMat_origin.channels() == num_channels);  // Weird to send it :D

  Mat imgMat;
  cv::resize(imgMat_origin, imgMat, Size(resize_width, resize_height));
  // add to db: 256 * 256 * 3 = 196608

  string* datum_string = datum.mutable_data();

  if (num_channels == 3) {
    for (int c = 0; c < num_channels; ++c) {
      for (int h = 0; h < imgMat.rows; ++h) {
        for (int w = 0; w < imgMat.cols; ++w) {
          datum_string->push_back(static_cast<uint8_t>(imgMat.at<cv::Vec3b>(h, w)[c]));
        }
      }
    }
  } else {
    for (int h = 0; h < imgMat.rows; ++h) {
      for (int w = 0; w < imgMat.cols; ++w) {
        datum_string->push_back(static_cast<uint8_t>(imgMat.at<uchar>(h, w)));
      }
    }
  }

  return true;
}

bool LeveldbWriter::addImageToDatum(const string& filename, int num_channels) {
  assert(!is_closed);

  if(is_virtual)  return true;

  int cv_read_flag = (num_channels == 3 ? CV_LOAD_IMAGE_COLOR : CV_LOAD_IMAGE_GRAYSCALE);

  Mat imgMat_origin = cv::imread(filename, cv_read_flag);

  if (!imgMat_origin.data) {
    LOG(ERROR)<< "Could not open or find file " << filename;
    return false;
  }
  return addImageToDatum(imgMat_origin, num_channels);
}

void LeveldbWriter::writeBatch() {
  if (is_closed)
    return;

  if(is_virtual)  return;

  if (countId == lastCountId)  // nothing changed
    return;

  leveldb::Status status = db->Write(leveldb::WriteOptions(), batch);
  CHECK(status.ok()) << "Failed to write the batch. Count id:  " << countId << "\n";

  delete batch;
  batch = new leveldb::WriteBatch();

  LOG(ERROR)<<db_path<<": Processed " << countId << " files.";
  lastCountId = countId;
}

void LeveldbWriter::forceFinalize() {
  if (is_closed)
    return;

  if(!is_virtual) {

    // write the last batch
    if (countId % WRITING_LIMIT != 0)
      writeBatch();

    if (batch != NULL)
      delete batch;

    if (db != NULL)
      delete db;
  }

  if (labels.size() == 1)  // Zero case, means caller not interested in setting labels. Just dummy labels.
    cerr << "\n\n\nThere is only ONE label in database. There should be a bug\n";

  cerr<<"\nLabels Statistics for db "<<db_path<<"\n";

  cerr<<"Total Records "<<labelsVec.size()<<"\n";

  cerr<<"*********************************************************\n";

  MostCV::getFrequencyMap(labelsVec, true);

  cerr<<"*********************************************************\n";

  MostCV::getFrequencyMapPercent(labelsVec, true);

  is_closed = true;
}

}
