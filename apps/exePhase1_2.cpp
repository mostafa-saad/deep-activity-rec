/*
 * w-driver-volleyball-lstm-evaluator.cpp
 *
 *  Created on: Jul 13, 2015
 *      Author: msibrahi
 */

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <vector>
#include <set>
#include <map>
using std::vector;
using std::set;
using std::map;
using std::pair;
using std::endl;
using std::cout;

#include "../src/leveldb-writer.h"
#include "../src/custom-macros.h"
#include "../src/rect-helper.h"
#include "../src/utilities.h"
#include "../src/images-utilities.h"
#include "../src/custom-images-macros.h"
#include "../src/dlib-tracker-wrapper.h"
#include "../src/volleyball-dataset-mgr.h"
using MostCV::VolleyballPerson;
using MostCV::VolleyballVideoData;
using MostCV::VolleyballDatasetPart;
using MostCV::VolleyballDatasetMgr;
using MostCV::RectHelper;

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

const int resize_width = 256;
const int resize_height = 256;
const int num_channels = 3;
const int kPlayersCount = 12;

/////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
  string program_name = MostCV::consumeStringParam(argc, argv);

  cerr << "Start: " << program_name << endl;
  // read program entry data
  string dataset_videos_path = MostCV::consumeStringParam(argc, argv);
  string config_path = MostCV::consumeStringParam(argc, argv);
  string leveldb_output_path = MostCV::consumeStringParam(argc, argv);
  int temporal_window = MostCV::consumeIntParam(argc, argv);
  int step = MostCV::consumeIntParam(argc, argv);
  int bIsPrepareLSTMData = MostCV::consumeIntParam(argc, argv); // otherwise fusion data

  if (bIsPrepareLSTMData)
    cerr << "LSTM 1 preparation" << endl;
  else
    cerr << "Data Fusion for LSTM 2" << endl;

  assert(temporal_window > 0);
  MostCV::fixDir(config_path);
  MostCV::fixDir(dataset_videos_path);
  MostCV::fixDir(leveldb_output_path);

  cerr << "Loading the dataset..." << endl;
  VolleyballDatasetMgr mgr(config_path, dataset_videos_path);

  cerr << "Temporal window = " << temporal_window << " with step = " << step << "\n\n";

  vector<Ptr<MostCV::LeveldbWriter> > dbMgrs;
  Mat blackRectImage = Mat::zeros(resize_width, resize_height, CV_8UC3);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Create leveldb datasets
  for (auto &dataset : mgr.dataset_division_) {
    dataset.dataset_db_name_ = dataset.dataset_name_ + "-leveldb";
    dataset.dataset_db_path_ = leveldb_output_path + dataset.dataset_db_name_;

    MostCV::fixDir(dataset.dataset_db_path_);

    cerr<<"Creating a new dataset\n";
    dbMgrs.push_back(new MostCV::LeveldbWriter(dataset.dataset_db_path_, resize_height, resize_width, num_channels, false));

    if (bIsPrepareLSTMData)
      dbMgrs.back()->setLabelsRange(mgr.total_persons_labels);
    else
      dbMgrs.back()->setLabelsRange(mgr.total_scene_labels);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  int dataset_pos = 0;
  boost::mt19937 generator(100);
  boost::uniform_int<> uni_dist;
  boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rand_generator(generator, uni_dist);

  for (auto dataset : mgr.dataset_division_) {

    // Shuffle data before use
    cerr << "Extracting shuffled elements from " << dataset.dataset_name_ << " Data Set. Total videos = " << dataset.videos_vec_.size() << "\n";

    Ptr<MostCV::LeveldbWriter> dbMgr = dbMgrs[dataset_pos++];
    vector<pair<VolleyballVideoData, string> > database_shuffled;

    for (auto video : dataset.videos_vec_) {
      for (auto frame_id : video.annot_frame_id_vec_)
        database_shuffled.push_back(std::make_pair(video, frame_id));
    }

    std::random_shuffle(database_shuffled.begin(), database_shuffled.end(), rand_generator);

    if (bIsPrepareLSTMData) {
      cerr << "Total images for current data set is " << database_shuffled.size() << ". Overall entries will be <= "
           << temporal_window * database_shuffled.size() * kPlayersCount << endl;
    } else {
      cerr << "Total images for current data set is " << database_shuffled.size() << ". Overall entries will be = "
           << temporal_window * database_shuffled.size() * kPlayersCount << endl;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    for (auto database_entry : database_shuffled) {
      auto video = database_entry.first;
      string frame_id = database_entry.second;
      int frame_label = video.annot_frame_id_to_activity_id_map_[frame_id];

      // prepare tracking data
      pair<vector<string>, vector<string> > images_paths_seq = video.GetTemporalWindowPaths(frame_id, temporal_window, step, false);

      vector<Mat> imagesSequenceBefore, imagesSequenceAfter;
      Mat img;

      for (auto path : images_paths_seq.first)
        imagesSequenceBefore.push_back(cv::imread(path));

      for (auto path : images_paths_seq.second)
        imagesSequenceAfter.push_back(cv::imread(path));

      if (imagesSequenceAfter.size())
        img = imagesSequenceAfter.back();
      else
        img = imagesSequenceBefore.back();

      assert(!img.empty());

      vector<VolleyballPerson> &persons = video.annot_frame_id_persons_map_[frame_id];
      vector<Mat> images;
      vector<vector<Rect> > persons_tracklets;

      for (auto person : persons) {
        MostCV::DlibTrackerWrapper tracker(person.bbox_.r);
        pair<vector<Mat>, vector<Rect> > tracklet = tracker.Process(imagesSequenceBefore, imagesSequenceAfter);

        images = tracklet.first;
        persons_tracklets.push_back(tracklet.second);
      }

      // generates temporal_window * kPlayersCount * frames
      int seq_id = 0, person_pos = 0;

      for (auto tracklet : persons_tracklets) {
        int rect_pos = 0;

        for (auto img : images) {
          dbMgr->clearDatum();
          //MostCV::ShowImage(img(tracklet[rect_pos]));
          assert(dbMgr->addImageToDatum(img(tracklet[rect_pos]), num_channels));

          if (bIsPrepareLSTMData)
            dbMgr->setDatumLabel(persons[person_pos].action_id_);
          else
            dbMgr->setDatumLabel(frame_label);

          dbMgr->addDatumToBatch(video.video_id_ + "_" + frame_id + "_" + MostCV::toIntStr("000", seq_id++));
          rect_pos++;
        }
        ++person_pos;
      }

      // for missing persons, add zero images
      if (!bIsPrepareLSTMData) {
        LP(j, kPlayersCount - persons_tracklets.size())
        {
          LP(k, temporal_window)
          {
            dbMgr->clearDatum();
            assert(dbMgr->addImageToDatum(blackRectImage, num_channels));
            dbMgr->setDatumLabel(frame_label);
            dbMgr->addDatumToBatch(video.video_id_ + "_" + frame_id + "_" + MostCV::toIntStr("000", seq_id++));
          }
        }
      }
    }
    dbMgr->forceFinalize();
  }

  cerr << "\n\nBye: " << program_name << endl;

  return 0;
}

