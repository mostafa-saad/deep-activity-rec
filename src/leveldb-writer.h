/*
 * LeveldbWriter.h
 *
 *  Created on: 2015-04-02
 *      Author: Moustafa S. Ibrahim
 */

#ifndef LeveldbWriter_H_
#define LeveldbWriter_H_

#include <string>
#include <set>
#include <vector>
using std::vector;
using std::set;
using std::string;

#include <glog/logging.h>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include "caffe/proto/caffe.pb.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using cv::Mat;
using cv::Size;


namespace MostCV {

class LeveldbWriter {
public:
  // Using zero parameters would mean not interested to add addImageToDatum functionality.
  LeveldbWriter(string db_path, int resize_height = -1, int resize_width = 1, int volumeSize = 1, bool is_virtual = false);
  ~LeveldbWriter();

  void clearDatum();
  void setDatumLabel(int id);
  bool addImageToDatum(const string& filename, int num_channels);
  bool addImageToDatum(Mat img, int num_channels);

  bool addVectorDatum(const vector<double> &feature_vec);
  void addDatumToBatch(string key = "");
  void addDatumToBatch(caffe::Datum &datum, string key, int label);

  void setLabelsRange(int max_label_cnt);
  void forceFinalize();

private:
  void writeBatch();


  leveldb::DB* db;
  leveldb::WriteBatch* batch;
  caffe::Datum datum;
  int countId;
  int lastCountId;

  string db_path;
  int resize_height;
  int resize_width;
  int volume_size;
  int internal_idx;

  set<int> labels;          //helps in verification.
  vector<int> labelsVec;    // print purposes
  int max_label_cnt;
  set<string> keys;
  bool is_closed;
  bool is_virtual;
};

}

#endif /* LeveldbWriter_H_ */
