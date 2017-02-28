/*
 * leveldb-reader.cpp
 *
 *  Created on: 2015-05-21
 *      Author: Moustafa S. Ibrahim
 */

#include<algorithm>
#include<iostream>
#include<fstream>

#include "leveldb-reader.h"
using std::ifstream;
using std::ofstream;
using std::endl;
using std::cout;

#include "utilities.h"

MostCV::LevelDBReader::LevelDBReader(const string & database_path, const string & sorted_list_file) {
  record_idx_ = 0;
  cache_limit_ = 1000;
  database_path_ = database_path;

  is_caching = true;

  if (sorted_list_file == "")
    is_caching = false;

  if (is_caching) {
    ifstream ifs(sorted_list_file.c_str());
    string line;

    assert(ifs.is_open());

    while (getline(ifs, line)) {
      int pos = line.find(' ');

      if (pos != -1)
        line = line.substr(0, pos);

      pos = line.find_last_of('/');

      if (pos != -1)
        line = line.substr(pos + 1);

      if (line != "")
        vectors_names_.push_back(line);
    }
    vector<string> images_names_temp = vectors_names_;
    std::sort(images_names_temp.begin(), images_names_temp.end());

    assert(images_names_temp == vectors_names_);
  }

  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, database_path_, &database_);
  assert(status.ok());

  database_iter_ = database_->NewIterator(leveldb::ReadOptions());
  assert(database_iter_ != NULL);

  database_iter_->SeekToFirst();
}

MostCV::LevelDBReader::~LevelDBReader() {
  if (database_iter_ != NULL)
    delete database_iter_;

  if (database_ != NULL)
    delete database_;
}

bool MostCV::LevelDBReader::GetNextEntry(string &key, vector<double> &retVec, int &label) {
  if (!database_iter_->Valid())
    return false;

  Datum datum;
  datum.clear_float_data();
  datum.clear_data();
  datum.ParseFromString(database_iter_->value().ToString());

  key = database_iter_->key().ToString();
  label = datum.label();

  int expected_data_size = std::max<int>(datum.data().size(), datum.float_data_size());
  const int datum_volume_size = datum.channels() * datum.height() * datum.width();
  if (expected_data_size != datum_volume_size) {
    cout << "Something wrong in saved data.";
    assert(false);
  }

  retVec.resize(datum_volume_size);

  const string& data = datum.data();
  if (data.size() != 0) {
    // Data stored in string, e.g. just pixel values of 196608 = 256 * 256 * 3
    for (int i = 0; i < datum_volume_size; ++i)
      retVec[i] = data[i];
  } else {
    // Data stored in real feature vector such as 4096 from feature extraction
    for (int i = 0; i < datum_volume_size; ++i)
      retVec[i] = datum.float_data(i);
  }

  database_iter_->Next();
  ++record_idx_;

  return true;
}

bool MostCV::LevelDBReader::GetNextEntryByKey(const string & name, vector<double> &retVec, int &label) {

  if (!is_caching) {
    cout << "A sorted file MUST be given. What are you trying to retrive!\n";
    assert(false);
  }

  if (cache_.count(name)) {
    retVec = cache_[name];
    return true;
  }

  string key;
  while (GetNextEntry(key, retVec, label)) {
    if ((int) cache_items_.size() == cache_limit_) {
      map<string, vector<double> >::iterator it = cache_.find(cache_items_.front());

      assert(it != cache_.end());
      cache_.erase(it);
      cache_items_.pop_front();
    }
    cache_[vectors_names_[record_idx_ - 1]] = retVec;
    cache_items_.push_back(vectors_names_[record_idx_ - 1]);

    if (vectors_names_[record_idx_ - 1] == name)
      return true;
  }

  cout << "Reached end of data: Total Records: " << record_idx_ << "\n";
  cout << "Failed to find data for: " << name << " in database path: " << database_path_ << "\n";

  assert(false);  // We failed to retrieve!

  return false;
}

void MostCV::LevelDBReader::Dump(const string & file_path, int featureVectorLimit) {
  record_idx_ = 0;
  database_iter_->SeekToFirst();

  ofstream ofs(file_path.c_str());

  vector<double> retVec;
  string key;
  int label;

  while (GetNextEntry(key, retVec, label)) {
    ofs << "key=" << key << ", label=" << label << ", features length=" << retVec.size();

    if (featureVectorLimit > 0) {
      ofs << ", truncated";
      retVec.resize(featureVectorLimit);  // To avoid writing much
    }

    ofs << ", feature vec= ";
    for (size_t i = 0; i < retVec.size(); ++i)
      ofs << retVec[i] << " ";
    ofs << "\n";
  }
  ofs.close();

  cout << "\nDump done: Total Records: " << record_idx_ << "\n";
}

void MostCV::LevelDBReader::DumpSmall(const string &file_path, int featureVectorLimit, bool make_random) {
  record_idx_ = 0;
  database_iter_->SeekToFirst();

  ofstream ofs(file_path.c_str());

  vector<double> retVec;

  string key;
  int label;

  for (int cnt = 0; cnt < 500 && GetNextEntry(key, retVec, label); ++cnt) {
    ofs << "key=" << key << ", label=" << label << ", features length=" << retVec.size();

    if (make_random)
      std::random_shuffle(retVec.begin(), retVec.end());

    if (featureVectorLimit > 0) {
      ofs << ", truncated";
      retVec.resize(featureVectorLimit);  // To avoid writing much
    }

    ofs << ", feature vec= ";
    for (size_t i = 0; i < retVec.size(); ++i)
      ofs << retVec[i] << " ";
    ofs << "\n";
  }
  ofs.close();

  cout << "\nDump done: Total Records: " << record_idx_ << "\n";
}

void MostCV::LevelDBReader::ReadLabels(vector<int> &labels, int max_rows) {
  record_idx_ = 0;
  database_iter_->SeekToFirst();

  labels.clear();

  string key;
  int label;
  vector<double> retVec;

  for (int row = 0; GetNextEntry(key, retVec, label); ++row) {
    if(max_rows != -1 && max_rows == row)
      break;
    labels.push_back(label);
  }
}

int MostCV::LevelDBReader::GetRecordsCount() {
  record_idx_ = 0;
  database_iter_->SeekToFirst();

  string key;
  int label;
  vector<double> retVec;

  while (GetNextEntry(key, retVec, label))
    ;

  return record_idx_;
}

void MostCV::LevelDBReader::SeekToHead() {
  record_idx_ = 0;
  database_iter_->SeekToFirst();
}



