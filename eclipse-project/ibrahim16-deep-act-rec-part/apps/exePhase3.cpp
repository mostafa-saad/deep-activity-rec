#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <set>
using std::vector;
using std::set;
using std::string;
using std::pair;
using std::endl;
using std::cout;

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
#include "../src/leveldb-writer.h"

enum fuse_style {
  concatenate_players = 0,
  max_pool_players_1 = 1,   //  all players in one vec of feature mid size
  max_pool_players_2 = 2,
  max_pool_players_4 = 3,   // divide the ground 4 blocks and max pool it. E.g. in 16 players, each 4 has max pool
  avg_pool_players_1 = 4,
  avg_pool_players_2 = 5,
  avg_pool_players_4 = 6,
  sum_pool_players_1 = 7,
  sum_pool_players_2 = 8,
  sum_pool_players_4 = 9
};

string fuse_style_sz[] = { "concatenate_players", "max_pool_players_1", "max_pool_players_2", "max_pool_players_4", "avg_pool_players_1", "avg_pool_players_2",
    "avg_pool_players_4", "sum_pool_players_1", "sum_pool_players_2", "sum_pool_players_4" };

int target_fuse_style = concatenate_players;
const int kPlayersCount = 12;









void RemoveLastBlock(vector<float> &input, int block_length) {
  assert((int )input.size() >= block_length);

  for (int i = 0; i < block_length; ++i)
    input.pop_back();
}

void AddLastBlock(vector<float> &input, int block_length) {

  for (int i = 0; i < block_length; ++i)
    input.push_back(0);
}

void RemoveDummyVectors(vector<float> &input, int block_length) {
  bool is_all_zeros = true;

  while (is_all_zeros && (int) input.size() > block_length) {  // Leave at least 1 block
    int last_idx = input.size() - 1;
    for (int i = 0; i < is_all_zeros && block_length; ++i)
      is_all_zeros &= input[last_idx - i] == 0;

    if (is_all_zeros)
      RemoveLastBlock(input, block_length);
  }
}

// target_blocks_cnt = 1 => merge all sub-vectors in 1 block
// target_blocks_cnt = 4 => merge every set of consecutive sub-vectors to get total 4 blocks
vector<float> VectorsFusing(vector<float> &input, int block_length, int target_blocks_cnt) {

  // I fixed bug here...hopefully not big problem!

  if (target_fuse_style == avg_pool_players_1 || target_fuse_style == sum_pool_players_1 || target_fuse_style == max_pool_players_1)
    RemoveDummyVectors(input, block_length);
  else if (target_fuse_style == concatenate_players) {
    int cur_blocks = input.size() / block_length;

    // then we need specific count of boxes
    assert(cur_blocks >= kPlayersCount);

    while (cur_blocks > kPlayersCount) {
      --cur_blocks;
      RemoveLastBlock(input, block_length);
    }
  } else {
    RemoveDummyVectors(input, block_length);

    while (input.size() > 0 && (input.size() % (block_length * target_blocks_cnt) != 0))
      AddLastBlock(input, block_length);
  }

  vector<float> output;
  const float* pData = &input[0];
  if (input.size() % (block_length * target_blocks_cnt) != 0) {
    cerr << "Error A%(B*C) != 0 => " << input.size() << " " << block_length << " " << target_blocks_cnt << "\n";
    assert(input.size() % (block_length * target_blocks_cnt) == 0);
  }
  int merge_blocks_cnt = input.size() / (block_length * target_blocks_cnt);  // merge cnt

  for (int i = 0; i < (int) input.size(); i += block_length * merge_blocks_cnt) {
    int t = merge_blocks_cnt;

    vector<float> sub_output(block_length);

    for (int j = 0; j < block_length; ++j)
      sub_output[j] = pData[j];

    pData += block_length;
    --t;

    while (t--) {
      for (int j = 0; j < block_length; ++j) {
        if (target_fuse_style == avg_pool_players_1 || target_fuse_style == avg_pool_players_2 || target_fuse_style == avg_pool_players_4
            || target_fuse_style == sum_pool_players_1|| target_fuse_style == sum_pool_players_2|| target_fuse_style == sum_pool_players_4)
          sub_output[j] += pData[j];
        else
          sub_output[j] = std::max(sub_output[j], pData[j]);
      }

      pData += block_length;
    }

    for (auto val : sub_output)
      output.push_back(val);
  }

  if (target_fuse_style == avg_pool_players_1 || target_fuse_style == avg_pool_players_2 || target_fuse_style == avg_pool_players_4) {
    for (auto &val : output)
      val /= merge_blocks_cnt;
  }

  return output;
}



template<typename Dtype>
void feature_extraction_pipeline(int &argc, char** &argv) {

  target_fuse_style = MostCV::consumeIntParam(argc, argv, "target_fuse_style");
  LOG(ERROR)<< "Fusing style = "<<fuse_style_sz[target_fuse_style] <<"\n\n";

  int frames_window = MostCV::consumeIntParam(argc, argv, "frames_window");
  LOG(ERROR)<< "frames_window = " << frames_window;

  LOG(ERROR)<< "Expected batch size = "<<kPlayersCount * frames_window;

  string computation_mode = MostCV::consumeStringParam(argc, argv);

  if (strcmp(computation_mode.c_str(), "GPU") == 0) {
    uint device_id = MostCV::consumeIntParam(argc, argv, "device_id");

    LOG(ERROR)<< "Using GPU";
    LOG(ERROR)<< "Using Device_id=" << device_id;

    Caffe::SetDevice(device_id);
    Caffe::set_mode(Caffe::GPU);
  } else {
    LOG(ERROR)<< "Using CPU";
    Caffe::set_mode(Caffe::CPU);
  }

  string pretrained_binary_proto(MostCV::consumeStringParam(argc, argv));
  string feature_extraction_proto(MostCV::consumeStringParam(argc, argv));

  LOG(ERROR)<<"Model: "<<pretrained_binary_proto;
  LOG(ERROR)<<"Proto: "<<feature_extraction_proto;

  LOG(ERROR)<<"Creating the test network\n";
  shared_ptr<Net<Dtype> > feature_extraction_net(new Net<Dtype>(feature_extraction_proto, caffe::Phase::TEST));

  LOG(ERROR)<<"Loading the Model\n";
  feature_extraction_net->CopyTrainedLayersFrom(pretrained_binary_proto);

  vector<string> blob_names_vec;

  int blobs_cnt = MostCV::consumeIntParam(argc, argv, "blobs_cnt");

  assert(blobs_cnt > 0);

  LOG(ERROR)<<"# of blobs is "<<blobs_cnt;

  LP(i, blobs_cnt)
  {
    string blob_name = MostCV::consumeStringParam(argc, argv);

    LOG(ERROR)<<"blob_name: "<<blob_name;

    CHECK(feature_extraction_net->has_blob(blob_name)) << "Unknown feature blob name " << blob_name << " in the network " << feature_extraction_proto;

    blob_names_vec.push_back(blob_name);
  }

  string output_dataset_name = MostCV::consumeStringParam(argc, argv);

  int num_mini_batches = MostCV::consumeIntParam(argc, argv, "num_mini_batches");

  LOG(ERROR)<<"num_mini_batches: "<<num_mini_batches;

  MostCV::LeveldbWriter leveldbWriter(output_dataset_name);

  Datum datum;
  const int kMaxKeyStrLength = 100;
  char key_str[kMaxKeyStrLength];
  vector<Blob<float>*> input_vec;
  int db_entry_idx = 0;
  int batch_size = -1;
  int dim_features = -1;

  std::set<int> batch_labels;        // all our batch value must be same
  std::set<int> dataset_labels;   // logically database shouldn't have only 1 label

  for (int batch_index = 0; batch_index < num_mini_batches; ++batch_index) {  // e.g. 100 iterations. Probably roll on data if needed
    feature_extraction_net->Forward(input_vec);  // Take one batch of data (e.g. 50 images), and pass them to end of network

    // Load the Labels
    const shared_ptr<Blob<Dtype> > label_blob = feature_extraction_net->blob_by_name("label");

    batch_size = label_blob->num();                   // e.g. 16 batches for volleyball..represents the boxes of a frame.

    assert(batch_size == frames_window * kPlayersCount);

    batch_labels.clear();
    int current_label = -1;

    for (int n = 0; n < batch_size; ++n) {
      const Dtype* label_blob_data = label_blob->cpu_data() + label_blob->offset(n);  // move offset to ith blob in batch
      current_label = label_blob_data[0];  // all will be same value
      batch_labels.insert(current_label);
      dataset_labels.insert(current_label);
    }

    if (batch_labels.size() != 1) {  // every 1 batch should have same value
      cerr << "\n\nERROR. Every 1 batch should have the same value. Inconsistent batch # " << batch_index + 1 << "-th\n";
      cerr << "Overall unique labels are: " << batch_labels.size() << ". The appeared labels are: ";

      for(auto label : batch_labels)
        cerr<<label<<" ";
      cerr<<"\n";
      assert(false);
    }
    vector<shared_ptr<Blob<Dtype> > > feature_blob_vec;

    for (auto blob_name : blob_names_vec) {
      shared_ptr<Blob<Dtype> > feature_blob = feature_extraction_net->blob_by_name(blob_name);  // get e.g. fc7 blob for the batch
      feature_blob_vec.push_back(feature_blob);
    }

    int total_dim_features = 0;
    static bool print_once_feature_vec = true;

    if (print_once_feature_vec)
      LOG(ERROR)<<"\n\n";

    for (auto feature_blob : feature_blob_vec) {
      dim_features = feature_blob->count() / batch_size;  // e.g. 4096
      total_dim_features += dim_features;                 // e.g. 4096 of fc7 + 250 of lstm1

      if (print_once_feature_vec)
        LOG(ERROR)<<"ith Vector Length = "<<dim_features;
      }

    vector<vector<float> > window_feature_vecs(frames_window);

    for (int n = 0; n < batch_size; ++n) {
      for (auto feature_blob : feature_blob_vec) {
        dim_features = feature_blob->count() / batch_size;  // e.g. 4096

        const Dtype* feature_blob_data = feature_blob->cpu_data() + feature_blob->offset(n);  // move offset to ith blob in batch

        int p = n % frames_window;
        for (int d = 0; d < dim_features; ++d)
          window_feature_vecs[p].push_back(feature_blob_data[d]);
      }
    }
    for (auto &feature_vec : window_feature_vecs) {
      if (target_fuse_style == max_pool_players_1 || target_fuse_style == avg_pool_players_1 || target_fuse_style == sum_pool_players_1)
        feature_vec = VectorsFusing(feature_vec, total_dim_features, 1);
      else if (target_fuse_style == max_pool_players_2 || target_fuse_style == avg_pool_players_2 || target_fuse_style == sum_pool_players_2)
        feature_vec = VectorsFusing(feature_vec, total_dim_features, 2);
      else if (target_fuse_style == max_pool_players_4 || target_fuse_style == avg_pool_players_4 || target_fuse_style == sum_pool_players_4)
        feature_vec = VectorsFusing(feature_vec, total_dim_features, 4);
      // otherwise, keep it concatenated

      if (print_once_feature_vec)
        LOG(ERROR)<<"Fused Vector Length = "<<feature_vec.size();

      print_once_feature_vec = false;

      datum.set_width(1);
      datum.set_channels(1);
      datum.clear_data();
      datum.clear_float_data();
      datum.set_height(feature_vec.size());

      for (int p = 0; p < (int) feature_vec.size(); ++p)
        datum.add_float_data(feature_vec[p]);

      int length = snprintf(key_str, kMaxKeyStrLength, "%010d", db_entry_idx);  // "%010d" BUG fix
      leveldbWriter.addDatumToBatch(datum, string(key_str, length), current_label);
      ++db_entry_idx;
      feature_vec.clear();
    }
  }
  leveldbWriter.forceFinalize();

  assert(dataset_labels.size() > 1);  // some variety make sense!
}








int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  MostCV::consumeStringParam(argc, argv);  // read program entry data

  LOG(ERROR)<< "Make sure to have LD_LIBRARY_PATH pointing to LSTM implementation in case of LSTM\n\n";

  // as long as chucks of data
  while (argc) {
    if (argc < 6) {
      LOG(ERROR)<< "At least 6 parameters expected\n";
      assert(false);
    }

    feature_extraction_pipeline<float>(argc, argv);
    LOG(ERROR)<< "\n\nSuccessfully extracted the features!\n\n";
  }

  return 0;
}
