/*
 * RectHelper.h
 *
 *  Created on: 2015-07-06
 *      Author: Moustafa S. Ibrahim
 */

#ifndef RECTHELPER_H_
#define RECTHELPER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
using std::vector;
using std::map;
using std::string;
using std::endl;
using std::cout;
using std::ifstream;
using std::ofstream;

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using cv::Mat;
using cv::Scalar;
using cv::Rect;
using cv::Point;
using cv::Size;

namespace MostCV {

class RectHelper {
public:

  RectHelper(Rect rect = Rect(0, 0, 0, 0), double score = -1);

  static vector<RectHelper> ToRectHelpers(const vector<Rect> &rectangles_vec);
  static vector<Rect> ToRects(const vector<RectHelper> &rectangles_vec);
  static void DrawRects(Mat img, const vector<RectHelper> &rectangles_vec, bool is_make_copy = true, bool is_show = true, Scalar color = Scalar(-1, -1, -1));
  static void SortByConfidence(vector<RectHelper> &rects);
  static void SortByTopLeftPoint(vector<RectHelper> &rects);
  static void FilterBelowConfidenceThreshold(vector<RectHelper> &rects, double conf_score_threshold);
  static map<string, vector<RectHelper> > LoadImagesRectangles(string path_x1_y1_w_h);
  static void WriteImagesRectangles(const map<string, vector<RectHelper> > &imageRectHelpersMap, string path_x1_y1_w_h);

  Rect r;
  double conf_score;
  string category;  // E.g. Car bbox
  int category_idx;
  Scalar color;   // For drawing

  Mat image;  // Image the rectangle belong to it
  string image_name;
  string image_path;
  string image_parent_path;
};

}

#endif /* RECTHELPER_H_ */
