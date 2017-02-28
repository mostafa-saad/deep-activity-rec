/*
 * coactivity-dataset-mgr.h
 *
 *  Created on: Nov 28, 2015
 *      Author: msibrahi
 */

#ifndef VOLLEYBALL_DATASET_MGR_H_FINAL_DATASET_
#define VOLLEYBALL_DATASET_MGR_H_FINAL_DATASET_

#include <string>
#include <vector>
#include <set>
using std::vector;
using std::set;
using std::string;
using std::pair;

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using cv::Mat;
using cv::Size;
using cv::Ptr;

#include "rect-helper.h"

namespace MostCV {


class VolleyballPerson {
 public:
  RectHelper  bbox_;
  int action_id_;
};

class VolleyballVideoData {
 public:
  VolleyballVideoData() {}
  VolleyballVideoData(string video_id, string video_dir);

  string GetFramePath(string frame_id, int shift = 0);
  pair< vector<string>, vector<string> > GetTemporalWindowPaths(string frame_id, int temporal_window, int step = 1, bool is_use_expend_factor = true);
  vector<string> GetTemporalWindowPathsMerged(string frame_id, int temporal_window, int step = 1);
  void ResetPersons(string frame_id, vector<RectHelper> rects);
  vector<RectHelper> GetPersonsRect(string frame_id);
  void SortPersonsPerFrames();
  void visualize();


  string video_id_;
  string video_dir_;

  vector<string>  annot_frame_id_vec_;
  map<string, int> annot_frame_id_to_activity_id_map_;
  map<string, pair<int, int>> annot_frame_id_to_min_max_persons_y_map_;

  map<string, vector<VolleyballPerson> > annot_frame_id_persons_map_;
};

class VolleyballDatasetPart {
 public:
  VolleyballDatasetPart() {}
  VolleyballDatasetPart(string dataset_name, string config_file, string videos_root_dir);
  void ReorderVideos(vector<string> video_ids);
  vector<pair<VolleyballVideoData, int> > GetVideoFrameList(bool is_shuffled, int subset_percent);
  void visualize();

  vector<string> ids_;
  vector<VolleyballVideoData> videos_vec_;
  string dataset_name_;

  string dataset_db_name_;
  string dataset_db_path_;

};

class VolleyballDatasetMgr {
 public:
  VolleyballDatasetMgr(string config_dir_path, string videos_root_dir);

  void VerifyDataAvailbility(int temporal_window);

  int GetActivityId(string video_id, string frame_id);
  vector<VolleyballPerson> GetPersons(string video_id, string frame_id);

  vector<VolleyballDatasetPart> dataset_division_;
  int total_videos_;
  int total_frames_;
  int total_scene_labels;
  int total_persons_labels;
};

}

#endif /* VOLLEYBALL_DATASET_MGR_H_FINAL_DATASET_ */
