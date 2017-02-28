/*
 * RectHelper.cpp
 *
 *  Created on: 2015-07-06
 *      Author: Moustafa S. Ibrahim
 */

#include "rect-helper.h"

#include "images-utilities.h"
#include "utilities.h"
#include "custom-macros.h"

namespace MostCV {

RectHelper::RectHelper(Rect rect, double score) {
  r = rect;
  conf_score = score;
  color = Scalar(rand() % 256, rand() % 256, rand() % 256); // random color
}

vector<RectHelper> RectHelper::ToRectHelpers(const vector<Rect> &rectangles_vec) {
  vector<RectHelper> ret;

  for(auto rect : rectangles_vec)
    ret.push_back(RectHelper(rect));

  return ret;
}

vector<Rect> RectHelper::ToRects(const vector<RectHelper> &rectangles_vec)
{
  vector<Rect> ret;

  for(auto rect : rectangles_vec)
    ret.push_back(rect.r);

  return ret;
}

//////////////////////////// Static Methods /////////////////////////////

void RectHelper::DrawRects(Mat img, const vector<RectHelper> &rectangles_vec, bool is_make_copy, bool is_show, Scalar color) {
  Mat imgTemp;

  if (is_make_copy) {
    img.copyTo(imgTemp);
    img = imgTemp;
  }

  for (auto rect_helper : rectangles_vec)
    cv::rectangle(img, rect_helper.r, (color[0] == -1) ? rect_helper.color : color, 2);

  int maxArea = 600 * 800;
  int dif = sqrt(img.rows * img.cols / maxArea);

  if(dif > 1)
  {
    Size size(img.cols / dif, img.rows / dif);
    Mat toImg;
    cv::resize(img, toImg, size);
    img = toImg;
  }

  MostCV::ShowImage(img, 0, is_show);
}

map<string, vector<RectHelper> > RectHelper::LoadImagesRectangles(string path_x1_y1_w_h){
  map<string, vector<RectHelper> > retMap;

  ifstream ifs(path_x1_y1_w_h);

  int cnt;
  string image_name;

  while(ifs>>image_name>>cnt)
  {
    vector<RectHelper> rectHelpers;

    while(cnt--)
    {
      double x, y, w, h;
      double score;
      ifs>>x>>y>>w>>h>>score;

      rectHelpers.push_back(RectHelper(Rect(x, y, w, h), score));
    }
    retMap[image_name] = rectHelpers;
  }
  ifs.close();

  return retMap;
}

void RectHelper::WriteImagesRectangles(const map<string, vector<RectHelper> > &image_rect_helpers_Map, string path_x1_y1_w_h)
{
  ofstream ofs(path_x1_y1_w_h);

  for (auto img_rects_pair : image_rect_helpers_Map)
  {
    ofs<<img_rects_pair.first<<" "<<img_rects_pair.second.size();

    for (auto rectHelper: img_rects_pair.second)
      ofs<<" "<<rectHelper.r.x<<" "<<rectHelper.r.y<<" "<<rectHelper.r.width<<" "<<rectHelper.r.height<<" "<<rectHelper.conf_score;
    ofs<<"\n";
  }
  ofs.close();
}

void RectHelper::FilterBelowConfidenceThreshold(vector<RectHelper> &rects, double conf_score_threshold)
{
  for (size_t i = 0; i < rects.size(); ++i) {
    if(MostCV::dcmp(rects[i].conf_score, conf_score_threshold) < 0)
    {
      rects.erase(rects.begin() + i);
      --i;
    }
  }
}


bool __CmpSortByConfidence(const RectHelper &a, const RectHelper& b)
{
  return MostCV::dcmp(a.conf_score, b.conf_score) < 0;
}

void RectHelper::SortByConfidence(vector<RectHelper> &rects)
{
  sort(RALL(rects), __CmpSortByConfidence);
}

bool __CmpSortByTopLeftPoint(const RectHelper &a, const RectHelper& b)
{
  int d = MostCV::dcmp(a.r.x, b.r.x);

  if(d != 0)
    return d < 0;
  return MostCV::dcmp(a.r.y, b.r.y) < 0;
}

void RectHelper::SortByTopLeftPoint(vector<RectHelper> &rects)
{
  sort(RALL(rects), __CmpSortByTopLeftPoint);
}

}
