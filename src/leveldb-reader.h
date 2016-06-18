/*
 * leveldb-reader.h
 *
 *  Created on: 2015-05-21
 *      Author: Moustafa S. Ibrahim
 */

/*
 * The file handles the reading of leveldb files. The database hold set of feature vectors of same length.
 */

#ifndef LEVELDB_READER_H_
#define LEVELDB_READER_H_

#include <stdio.h>

#include <string>
#include <vector>
#include <deque>
#include <cassert>
#include <iostream>
#include <fstream>
#include <map>
using std::map;
using std::deque;
using std::vector;
using std::string;
using std::endl;
using std::cout;

#include <google/protobuf/text_format.h>
#include <glog/logging.h>

#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include "caffe/proto/caffe.pb.h"
#include "caffe/blob.hpp"
#include "caffe/common.hpp"
#include "caffe/net.hpp"
#include "caffe/proto/caffe.pb.h"
#include "caffe/util/db.hpp"
#include "caffe/util/io.hpp"
#include "caffe/vision_layers.hpp"

using caffe::Blob;
using caffe::Caffe;
using caffe::Datum;
using caffe::Net;

namespace MostCV {

/*
 * The class opens a leveldb directory, which has set of feature vectors (e.g. extracted by feature_extract tool from caffe tool).
 * In order, each feature vector has a name is a given in a "sorted" file.
 * User either can retrieve all feature vectors in order or filter based on name.
 *
 * User is expected to use one type only of the GetNextEntry methods. Similarly, if user used Dump method, shouldn't try to use other methods.
 * Reason behind such limitation: All the methods seek in the file. E.g., after dumping, no more rows to read.
 *
 * Usage Example:
 *
 * LevelDBReader reader(database_path, sorted_images_list_file);
 * vector<double> feature_vector;
 *
 * while(reader.GetNextEntry())
 *  doSomething(feature_vector);
 *
 */
class LevelDBReader {
public:
  /*
   * Open and prepare the database for reading. The database is allowed to have more rows than the file such that extra rows has no corresponding name.
   *
   * The file names should be sorted. Reason behind that is allowing efficient retrieval (e.g. using caching to last 200 rows). As a result, leveldb should be sorted too based on this key.
   *
   * In case no file given, then Just retrieve sequentially from DB. This is more suitable for dumping purposes.
   */
  LevelDBReader(const string & database_path, const string & sorted_list_file = "");
  ~LevelDBReader();

  // Read the next entry from the file. If no more rows, return false.
  bool GetNextEntry(string &key, vector<double> &retVec, int &label);

  // Given entry name from the sorted_images_list_file, return corresponding vector. Consecutive calls should be ordered in name.
  //    If not so, it shouldn't be far from the last sorted element to be retrieved from caching. We cache last X elements.
  bool GetNextEntryByKey(const string & name, vector<double> &retVec, int &label);

  // For debugging purposes, dump the database to a file. Truncate after the first "limit" elements.
  void Dump(const string &file_path, int featureVectorLimit = -1);
  void DumpSmall(const string &file_path, int featureVectorLimit = -1, bool make_random = true);
  void ReadLabels(vector<int> &labels, int max_rows = -1);
  int GetRecordsCount();
  void SeekToHead();

private:
  bool is_caching;
  vector<string> vectors_names_;
  string database_path_;

  leveldb::DB* database_;
  leveldb::Iterator* database_iter_;

  // Caching Variables
  map<string, vector<double> > cache_;
  deque<string> cache_items_;
  int cache_limit_;

  // Current row index in retrieval
  int record_idx_;
};

}

#endif /* LEVELDB_READER_H_ */
