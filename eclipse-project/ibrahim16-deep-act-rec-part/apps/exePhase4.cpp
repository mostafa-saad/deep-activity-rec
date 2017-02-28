/*
 * w-driver-volleyball-lstm-evaluator.cpp
 *
 *  Created on: Jul 13, 2015
 *      Author: msibrahi
 */

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string>
#include <set>
#include <set>
#include <map>
#include <iomanip>
using std::vector;
using std::set;
using std::multiset;
using std::map;
using std::pair;
using std::string;
using std::endl;
using std::cerr;

#include "boost/algorithm/string.hpp"
#include "google/protobuf/text_format.h"

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
using caffe::Layer;
using caffe::LayerParameter;
using caffe::DataParameter;
using caffe::NetParameter;
using boost::shared_ptr;
namespace db = caffe::db;

#include "../src/utilities.h"
#include "../src/leveldb-reader.h"

void evaluate(vector<int> truthLabels, vector<int> resultLabels, int w) {

  set<int> total_labels;
  map<int, map<int, int> > confusion_freq_maps;
  map<int, int> label_freq;
  int correct = 0;

  cerr<<"\n\n";
  for (int i = 0; i < (int) truthLabels.size(); ++i) {
    correct += truthLabels[i] == resultLabels[i];

    cerr << "Test " << i + 1 << ": Result = " << resultLabels[i] << " GroundTruth = " << truthLabels[i] << "\n";

    confusion_freq_maps[truthLabels[i]][resultLabels[i]]++;
    total_labels.insert(truthLabels[i]);
    total_labels.insert(resultLabels[i]);
    label_freq[truthLabels[i]]++;
  }

  cerr.setf(std::ios::fixed);
  cerr.precision(2);

  cerr<<"\n\n";
  cerr << "Total testing frames: " << truthLabels.size() << " with temporal window: " << w << "\n";
  cerr << "Temporal accuracy : " << 100.0 * correct / truthLabels.size() << " %\n";
  cerr << "\n=======================================================================================\n";

  cerr << "\nConfusion Matrix - Truth (col) / Result(row)\n\n";

  cerr << std::setw(5) << "T/R" << ": ";

  for (auto r_label : total_labels)
    cerr << std::setw(5) << r_label;
  cerr << "\n=======================================================================================\n";

  for (auto t_label : total_labels) {
    int sum = 0;
    cerr << std::setw(5) << t_label << ": ";

    for (auto r_label : total_labels)
    {
      cerr << std::setw(5) << confusion_freq_maps[t_label][r_label];

      sum += confusion_freq_maps[t_label][r_label];
    }

    double percent = 0;

    if (label_freq[t_label] > 0)
      percent = 100.0 * confusion_freq_maps[t_label][t_label] / label_freq[t_label];

    cerr << " \t=> Total Correct = " << std::setw(5) << confusion_freq_maps[t_label][t_label] << " / " << std::setw(5) << sum << " = " << percent << " %\n";
  }


  cerr<<"\n\n";
  cerr << std::setw(7) << "T/R" << ": ";

  for (auto r_label : total_labels)
    cerr << std::setw(7) << r_label;
  cerr << "\n=======================================================================================\n";

  for (auto t_label : total_labels) {
    cerr << std::setw(7) << t_label << ": ";

    for (auto r_label : total_labels)
    {
      double percent = 0;

      if (label_freq[t_label] > 0)
        percent = 100.0 * confusion_freq_maps[t_label][r_label] / label_freq[t_label];

      cerr << std::setw(7) << percent;
    }
    cerr<<"\n";
  }

  cerr<<"\nTo get labels corresponding to IDs..see dataset loading logs\n";
}

int getArgmax(vector<float> &v) {
  int pos = 0;

  assert(v.size() > 0);

  for (int j = 1; j < (int) v.size(); ++j) {
    if (v[j] > v[pos])
      pos = j;
  }
  return pos;
}

template<typename Dtype>
void feature_extraction_pipeline(int &argc, char** &argv) {

  int frames_window = MostCV::consumeIntParam(argc, argv);
  LOG(ERROR)<< "Temporal Window = " << frames_window;

  string computation_mode = MostCV::consumeStringParam(argc, argv);

  if (strcmp(computation_mode.c_str(), "GPU") == 0) {
    uint device_id = MostCV::consumeIntParam(argc, argv);

    LOG(ERROR)<< "Using GPU";
    LOG(ERROR)<< "Using Device_id = " << device_id;

    Caffe::SetDevice(device_id);
    Caffe::set_mode(Caffe::GPU);
  } else {
    LOG(ERROR)<< "Using CPU";
    Caffe::set_mode(Caffe::CPU);
  }

  string pretrained_binary_proto(MostCV::consumeStringParam(argc, argv));
  string feature_extraction_proto(MostCV::consumeStringParam(argc, argv));

  LOG(ERROR)<<"Model: "<<pretrained_binary_proto<<"\n";
  LOG(ERROR)<<"Proto: "<<feature_extraction_proto<<"\n";

  LOG(ERROR)<<"Creating the test network\n";
  shared_ptr<Net<Dtype> > feature_extraction_net(new Net<Dtype>(feature_extraction_proto, caffe::Phase::TEST));

  LOG(ERROR)<<"Loading the Model\n";
  feature_extraction_net->CopyTrainedLayersFrom(pretrained_binary_proto);

  string blob_name = MostCV::consumeStringParam(argc, argv);
  LOG(ERROR)<<"blob_name: "<<blob_name<<"\n";

  CHECK(feature_extraction_net->has_blob(blob_name)) << "Unknown feature blob name " << blob_name << " in the network " << feature_extraction_proto;

  int num_mini_batches = MostCV::consumeIntParam(argc, argv);
  LOG(ERROR)<<"num_mini_batches: "<<num_mini_batches<<"\n";

  vector<Blob<float>*> input_vec;
  int batch_size = -1;
  int dim_features = -1;
  std::set<int> labels;       // every (2w+1) * batch size MUST all have same label

  vector<int> truthLabels;
  vector<int> propAvgMaxResultLabels;

  for (int batch_index = 0; batch_index < num_mini_batches; ++batch_index) {  // e.g. 100 iterations. Probably roll on data if needed
    feature_extraction_net->Forward(input_vec);  // Take one batch of data (e.g. 50 images), and pass them to end of network

    // Load the Labels
    const shared_ptr<Blob<Dtype> > label_blob = feature_extraction_net->blob_by_name("label");

    batch_size = label_blob->num();                   // e.g. 50 batches

    assert(batch_size == frames_window);

    int current_label = -1;
    for (int n = 0; n < batch_size; ++n) {
      const Dtype* label_blob_data = label_blob->cpu_data() + label_blob->offset(n);  // move offset to ith blob in batch
      current_label = label_blob_data[0];  // all will be same value
      labels.insert(current_label);

      if (n == 0)
        truthLabels.push_back(current_label);
    }

    if (labels.size() != 1) {  // every 1 batch should have same value
      LOG(ERROR)<< "Something wrong. every 1 batch should have same value. New value at element " << batch_index + 1 << "\n";
      assert(false);
    }
    labels.clear();

    const shared_ptr<Blob<Dtype> > feature_blob = feature_extraction_net->blob_by_name(blob_name);  // get e.g. fc7 blob for the batch

    dim_features = feature_blob->count() / batch_size;
    assert(dim_features > 1);

    const Dtype* feature_blob_data = nullptr;

    vector<float> test_case_sum(dim_features);

    for (int n = 0; n < batch_size; ++n) {
      feature_blob_data = feature_blob->cpu_data() + feature_blob->offset(n);  // move offset to ith blob in batch

      vector<float> test_case;
      for (int j = 0; j < dim_features; ++j) {
        test_case.push_back(feature_blob_data[j]);

        test_case_sum[j] += feature_blob_data[j];
      }
    }

    propAvgMaxResultLabels.push_back( getArgmax(test_case_sum) );
  }

  evaluate(truthLabels, propAvgMaxResultLabels, 1);
}

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  MostCV::consumeStringParam(argc, argv);  // read program entry data

  if (argc < 6) {
    LOG(ERROR)<< "At least 6 parameters expected\n";
    assert(false);
  }

  LOG(ERROR)<< "Make sure to have LD_LIBRARY_PATH pointing to LSTM implementation in case of LSTM\n\n";

  feature_extraction_pipeline<float>(argc, argv);

  return 0;
}
