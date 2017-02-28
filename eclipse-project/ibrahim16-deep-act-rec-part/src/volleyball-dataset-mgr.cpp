/*
 * volleyball-dataset-mgr.cpp
 *
 *  Created on: Nov 28, 2015
 *      Author: msibrahi
 */

#include "volleyball-dataset-mgr.h"

#include <boost/filesystem.hpp>
namespace bst_fs = boost::filesystem;

#include "utilities.h"
#include "images-utilities.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

namespace MostCV {

map<string, int> global_video_id_frame_id_to_activityId;
map<string, vector<VolleyballPerson> > global_video_id_frame_id_to_persons;

map<string, int> persons_actions_ids_map;
map<string, int> scene_activities_ids_map;

// statistics
map<string, int> scene_activities_freq_map;
map<string, int> players_activities_freq_map;

VolleyballVideoData::VolleyballVideoData(string video_id, string video_dir) {
  MostCV::fixDir(video_dir);

  video_id_ = video_id;
  video_dir_ = video_dir;

  string annot_file = video_dir + "annotations.txt";
  vector<vector<string> > data2dVec = MostCV::read2dFileItems(annot_file, string(""), false);

  // For every frame, read the players in it
  for (auto frame_data : data2dVec) {
    VolleyballPerson person;

    string frame_id = frame_data[0];

    GetFramePath(frame_id); // verify on hard disk

    frame_data.erase(frame_data.begin());

   // if (frame_data[0].find("win") == string::npos)
   //   continue;

    scene_activities_freq_map[ frame_data[0] ]++;

    int frame_activity_id = MostCV::UpdateStringIdMap(scene_activities_ids_map, frame_data[0]);
    annot_frame_id_to_activity_id_map_[frame_id] = frame_activity_id;
    frame_data.erase(frame_data.begin());

    pair<int, int> min_max_persons_y = { 10000, 0 };

    for (int k = 0; k < (int) frame_data.size(); k += 5) {
      int x = MostCV::toType(frame_data[k + 0], 0);
      int y = MostCV::toType(frame_data[k + 1], 0);
      int w = MostCV::toType(frame_data[k + 2], 0);
      int h = MostCV::toType(frame_data[k + 3], 0);
      string activity_str = frame_data[k + 4];

      players_activities_freq_map[activity_str]++;

      min_max_persons_y.first = std::min(min_max_persons_y.first, y);
      min_max_persons_y.second = std::max(min_max_persons_y.second, y + h);

      person.bbox_ = RectHelper(Rect(x, y, w, h));
      person.action_id_ = MostCV::UpdateStringIdMap(persons_actions_ids_map, activity_str);

      annot_frame_id_persons_map_[frame_id].push_back(person);
    }



    if (min_max_persons_y.first < 0)
      min_max_persons_y.first = 0;

    annot_frame_id_to_min_max_persons_y_map_[frame_id] = min_max_persons_y;
    annot_frame_id_vec_.push_back(frame_id);

    string video_id_frame_id = video_id + "#"+frame_id;

    global_video_id_frame_id_to_activityId[video_id_frame_id] = frame_activity_id;
    global_video_id_frame_id_to_persons[video_id_frame_id] = annot_frame_id_persons_map_[frame_id];

    if (annot_frame_id_persons_map_[frame_id].size() < 7)
    {
    	cerr<<"video "<<video_id_frame_id<<" frame id "<<frame_id
    	    <<" has "<<annot_frame_id_persons_map_[frame_id].size()<<" persons\n";
    }

    if (annot_frame_id_persons_map_[frame_id].size() > 12)
        {
          cerr<<"video "<<video_id_frame_id<<" frame id "<<frame_id
              <<" has "<<annot_frame_id_persons_map_[frame_id].size()<<" persons!!\n";
        }
  }

  SortPersonsPerFrames();

  cerr << video_id_ << " is processed\n";
}

void VolleyballVideoData::SortPersonsPerFrames() {
  // Sorting the persons based on top left point: x first, if tie, y first. Kind of left-to-right sweeping
  for (auto &frame_persons_kv : annot_frame_id_persons_map_) {
    vector<VolleyballPerson> &persons = frame_persons_kv.second;

    sort(persons.begin(), persons.end(), [](const VolleyballPerson &a, const VolleyballPerson &b)
    {
      if(a.bbox_.r.x != b.bbox_.r.x)
      return a.bbox_.r.x < b.bbox_.r.x;
      return a.bbox_.r.y < b.bbox_.r.y;
    });
  }
}

void VolleyballVideoData::ResetPersons(string img_name, vector<RectHelper> rects) {
  annot_frame_id_persons_map_[img_name].clear();

  for (auto rect : rects) {
    VolleyballPerson person;

    person.bbox_ = rect;
    person.action_id_ = 0;

    annot_frame_id_persons_map_[img_name].push_back(person);
  }
}

vector<RectHelper> VolleyballVideoData::GetPersonsRect(string frame_id) {
  vector<RectHelper> rects;

  for (auto person : annot_frame_id_persons_map_[frame_id])
    rects.push_back(person.bbox_);

  return rects;
}

// Short Util
string VolleyballVideoData::GetFramePath(string frame_id, int shift) {
  string frame_id_no_ext = frame_id.substr(0, frame_id.find_first_of('.'));
  string ext = frame_id.substr(frame_id.find_first_of('.'));
  string target_frame_id = MostCV::toIntStr(frame_id_no_ext, shift, false);
  string frame_new_path = video_dir_ + frame_id_no_ext + MostCV::PATH_SEP + target_frame_id + ext;

  assert(boost::filesystem::exists(frame_new_path));

  return frame_new_path;
}

pair<vector<string>, vector<string> > VolleyballVideoData::GetTemporalWindowPaths(string frame_id, int temporal_window, int step, bool is_use_expend_factor) {
  vector<string> window_frames_after;
  vector<string> window_frames_before;

  if (is_use_expend_factor)
    temporal_window = 2 * temporal_window + 1;

  LP(w, 1+temporal_window/2)
  {
    string path = GetFramePath(frame_id, -w * step);
    window_frames_before.push_back(path);
  }

  LP(w, (temporal_window+1)/2)
  {
    string path = GetFramePath(frame_id, w * step);
    window_frames_after.push_back(path);
  }

  return {window_frames_before, window_frames_after};
}

vector<string> VolleyballVideoData::GetTemporalWindowPathsMerged(string frame_id, int temporal_window, int step) {
  vector<string> paths;

  int start = -temporal_window/2;

  LP(w, temporal_window)
  {
    string path = GetFramePath(frame_id, start * step);
    paths.push_back(path);
    ++start;
  }
  return paths;
}

void VolleyballVideoData::visualize()
{
  for (auto frame_id : annot_frame_id_vec_)
  {
    string path = GetFramePath(frame_id);
    Mat img = cv::imread(path);

    cerr<<video_id_<<" "<<frame_id<<" "<<path<<"\n";
    RectHelper::DrawRects(img, GetPersonsRect(frame_id));
  }
}









//---------------------------------------------------------------

VolleyballDatasetPart::VolleyballDatasetPart(string dataset_name, string config_file, string videos_root_dir) {

  cerr << "Preparing Dataset: " << dataset_name << "\n\tfrom config file: " << config_file << "\n";

  assert(MostCV::IsPathExist(config_file));

  MostCV::fixDir(videos_root_dir);
  ids_ = MostCV::readFileItems(config_file, string(""), false);
  dataset_name_ = dataset_name;

  for (auto video_seq : ids_)
	  videos_vec_.push_back(VolleyballVideoData(video_seq, videos_root_dir + video_seq));

  cerr << "\n\n************************\n\n";
}

void VolleyballDatasetPart::ReorderVideos(vector<string> video_ids) {
  for (int i = 0; i < (int) video_ids.size(); ++i) {
    for (int j = 0; j < (int) ids_.size(); ++j) {
      if (video_ids[i] != ids_[j])
        continue;
      std::swap(ids_[i], ids_[j]);
      std::swap(videos_vec_[i], videos_vec_[j]);
    }
  }
}

vector<pair<VolleyballVideoData, int> > VolleyballDatasetPart::GetVideoFrameList(bool is_shuffled, int subset_percent) {
  vector<pair<VolleyballVideoData, int> > database_shuffled;

  return database_shuffled;

  boost::mt19937 generator(100);
  boost::uniform_int<> uni_dist;
  boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rand_generator(generator, uni_dist);

  vector<int> labels;

  for (auto video : videos_vec_) {
    int frame_pos = -1;

    for (auto frame_id : video.annot_frame_id_vec_) {
      ++frame_pos;

      database_shuffled.push_back(std::make_pair(video, frame_pos));
    }
  }

  if (is_shuffled) {
    cerr << "Before: Total Shuffled Elements: " << database_shuffled.size() << " with 1st video" << database_shuffled.begin()->first.video_id_ << "\n";

    std::random_shuffle(database_shuffled.begin(), database_shuffled.end(), rand_generator);

    cerr << "After: Total Shuffled Elements: " << database_shuffled.size() << " with 1st video" << database_shuffled.begin()->first.video_id_ << "\n";
  }

  int max_size = subset_percent * database_shuffled.size();
  database_shuffled.resize(max_size);

  return database_shuffled;
}

void VolleyballDatasetPart::visualize()
{
  for (auto video : videos_vec_)
    video.visualize();
}








//---------------------------------------------------------------

VolleyballDatasetMgr::VolleyballDatasetMgr(string config_dir_path, string videos_root_dir) {
  MostCV::fixDir(config_dir_path);

  dataset_division_.push_back(VolleyballDatasetPart("train", config_dir_path + "train.txt", videos_root_dir));
  dataset_division_.push_back(VolleyballDatasetPart("val", config_dir_path + "val.txt", videos_root_dir));
  dataset_division_.push_back(VolleyballDatasetPart("test", config_dir_path + "test.txt", videos_root_dir));
  dataset_division_.push_back(VolleyballDatasetPart("trainval", config_dir_path + "trainval.txt", videos_root_dir));

  total_videos_ = 0;
  total_frames_ = 0;

  // Remove empty datasets
  for (int i = 0; i < (int) dataset_division_.size(); ++i) {
    if (dataset_division_[i].videos_vec_.size() == 0) {
      cerr << dataset_division_[i].dataset_name_ << " dataset is EMPTY\n";

      dataset_division_.erase(dataset_division_.begin() + i);
      --i;
    }
  }

  assert(dataset_division_.size() > 0);

  for (auto dataset : dataset_division_) {
    int current_fames = 0;

    for (auto video : dataset.videos_vec_) {
      total_frames_ += video.annot_frame_id_vec_.size();
      current_fames += video.annot_frame_id_vec_.size();
    }
    cerr << "Total frames for dataset " << dataset.dataset_name_ << " = " << current_fames << "\n";

    total_videos_ += dataset.videos_vec_.size();
  }

  total_scene_labels = scene_activities_ids_map.size();
  total_persons_labels = persons_actions_ids_map.size();

  cerr << "\nTotal videos = " << total_videos_ << " - total frames = " << total_frames_ << "\n";

  cerr << "\nScenes Labels:\n";
  for (auto scene_kv : scene_activities_ids_map)
    cerr << "\t" << scene_kv.first << " " << scene_kv.second << "\n";

  cerr << "\nPersons Labels:\n";
  for (auto persons_kv : persons_actions_ids_map)
    cerr << "\t" << persons_kv.first << " " << persons_kv.second << "\n";

  cerr << "\nScenes Labels frequency:\n";
  for (auto entry : scene_activities_freq_map)
    cerr << "\t" << entry.first << " " << entry.second << "\n";

  cerr << "\nPlayers Labels frequency:\n";
  for (auto entry : players_activities_freq_map)
    cerr << "\t" << entry.first << " " << entry.second << "\n";
}

int VolleyballDatasetMgr::GetActivityId(string video_id, string frame_id)
{
  string video_id_frame_id = video_id + "#"+frame_id;

 if (global_video_id_frame_id_to_activityId.count(video_id_frame_id) == 0)
 {
   cerr<<"problem with "<<video_id_frame_id<<"\n\n";
   return -1;
 }

  assert( global_video_id_frame_id_to_activityId.count(video_id_frame_id) );

  return global_video_id_frame_id_to_activityId[video_id_frame_id];
}

vector<VolleyballPerson> VolleyballDatasetMgr::GetPersons(string video_id, string frame_id)
{
  string video_id_frame_id = video_id + "#"+frame_id;

  assert( global_video_id_frame_id_to_persons.count(video_id_frame_id) );

  return global_video_id_frame_id_to_persons[video_id_frame_id];

}


// verify 2*w+1 elements..e.g. centered around every frame
void VolleyballDatasetMgr::VerifyDataAvailbility(int temporal_window)
{
  for (auto dataset : dataset_division_) {
    cerr<<"Verifying dataset: "<<dataset.dataset_db_name_<<"\n";
    for (auto video : dataset.videos_vec_) {
      for (auto frame_id : video.annot_frame_id_vec_) {
        video.GetTemporalWindowPaths(frame_id, temporal_window, 1, true);
      }
    }
  }
}


}
