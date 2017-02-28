#include "images-utilities.h"

#include <iostream>
using std::cout;

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "custom-images-macros.h"
#include "custom-macros.h"

namespace MostCV {

void ShowImage(Mat image, int wait, bool bShow, string stringWindowName) {
  if (bShow) {
    cv::namedWindow(stringWindowName.c_str(), 1);
    cv::imshow(stringWindowName.c_str(), image);
    cv::waitKey(wait);
  }
}

void RemoveImagePixels(Mat img, Mat mask, bool is_mask_remove_pixel_black, Point shift) {
  REPIMG2(y, x, mask)
    {
      if (mask.at<uchar> (y, x) == 0 && !is_mask_remove_pixel_black)
        continue;

      if (mask.at<uchar> (y, x) > 0 && is_mask_remove_pixel_black)
        continue;

      if (img.channels() == 3) {
        for (int c = 0; c < 3; ++c)
          img.at<cv::Vec3b> (y + shift.y, x + shift.x)[c] = 0;
      } else
        img.at<uchar> (y + shift.y, x + shift.x) = 0;
    }
}

void FixMask(Mat mask, int threshold) {
  int cnt = 0;

  REPIMG2(y, x, mask)
    {
      if (mask.at<uchar> (y, x) >= threshold) {
        if (mask.at<uchar> (y, x) != 255)
          cnt++;
        mask.at<uchar> (y, x) = 255;
      } else {
        if (mask.at<uchar> (y, x) != 0)
          cnt++;
        mask.at<uchar> (y, x) = 0;
      }
    }
  //if(cnt)    cout<<"FixMask: "<<cnt<<" pixels\n";
}

void Morphology(Mat mask, bool do_open, bool do_close, int open_kernel_sz, int close_kernel_sz) {

  Mat open_element = cv::getStructuringElement(0, Size(open_kernel_sz, open_kernel_sz));
  Mat close_element = cv::getStructuringElement(0, Size(close_kernel_sz, close_kernel_sz));

  if (do_open)
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, open_element);

  if (do_close)
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, close_element);
}

bool AddButton(Mat controlsMat, string buttonName, vector<Rect> &rectsSoFar, Scalar color) {
  int lastY = 0;
  int lastX = 0;
  Rect imgRect = Rect(0, 0, controlsMat.cols - 1, controlsMat.rows - 1);

  if (rectsSoFar.size()) {
    Rect r = rectsSoFar.back();
    lastY = r.y + r.height + 5;
    lastX = r.x;
  }
  Rect r(lastX, lastY, 100, 30);

  if ((r & imgRect) != r) {
    lastY = 0;
    lastX = r.x + r.width + 5;
    r = Rect(lastX, lastY, 100, 30);

    if ((r & imgRect) != r)
      return false;
  }

  cv::rectangle(controlsMat, r, Scalar(255, 255, 255), 2);
  cv::putText(controlsMat, buttonName, Point(r.x + 2, r.y + r.height / 2), cv::FONT_HERSHEY_SIMPLEX, 0.5, color);

  rectsSoFar.push_back(r);

  return true;
}

vector<Ptr<CComponenets> > GetConnectedComponenets(Mat img, int area_threshold, int pixels_threshold, Scalar lo_diff, Scalar up_diff, int flags) {

  assert(area_threshold > 0 && pixels_threshold > 0);

  Mat uchar_img;
  Rect img_rect(0, 0, img.cols - 1, img.rows - 1);
  vector<Ptr<CComponenets> > componenets;

  if (img.channels() > 1)
    cvtColor(img, uchar_img, CV_BGR2GRAY);
  else
    img.copyTo(uchar_img);

  REPIMG2(y, x, uchar_img)
    {
      int pixel_value = (int) uchar_img.at<uchar> (y, x);

      if (pixel_value < 1)
        continue;

      Rect rect;
      Mat mask = Mat::zeros(uchar_img.rows + 2, uchar_img.cols + 2, CV_8UC1);

      int mask_pixels_cnt = floodFill(uchar_img, mask, Point(x, y), Scalar(0), &rect, lo_diff, up_diff, flags);

      rect &= img_rect;

      if (rect.area() >= area_threshold && mask_pixels_cnt >= pixels_threshold) {
        Ptr<CComponenets> component = new CComponenets();

        MostCV::FixMask(mask);

        componenets.push_back(component);
        component->mask = mask(Rect(1, 1, uchar_img.cols, uchar_img.rows));
        component->mask_pixels_cnt = mask_pixels_cnt;
        component->rect = rect;
        component->flood_starting_point = Point(x, y);
        component->parent_mask_topleft_point = Point(0, 0);
      }
    }
  return componenets;
}

Rect GetInternalBlobRect(Mat mask)
{
  assert(mask.type() == CV_8UC1);

  vector<Ptr<MostCV::CComponenets> > comps = MostCV::GetConnectedComponenets(mask);

  if(comps.size() == 0)
    return Rect(0, 0, 1, 1);

  Rect union_rect = comps[0]->rect;

  REP(i, comps)
    union_rect |= comps[i]->rect;

  return union_rect;
}

vector<Point> GetCombinedContour(Mat mask) {
  vector<vector<Point> > contours;
  vector<cv::Vec4i> hierarchy;
  Mat componentCpy;

  mask.copyTo(componentCpy);
  cv::findContours(componentCpy, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

  vector<Point> contoursInOne;

  REP(j, contours)
    contoursInOne.insert(contoursInOne.end(), contours[j].begin(), contours[j].end());

  return contoursInOne;
}

Rect GetRect(Mat img)
{
  return Rect(0, 0, img.cols-1, img.rows-1);
}

void CenterRect(Rect &target_rect, int width, int height)
{
  if(width > target_rect.width)
  {
    target_rect.x -= (width - target_rect.width)/2;
    target_rect.width = width;
  }

  if(height > target_rect.height)
  {
    target_rect.y -= (height - target_rect.height)/2;
    target_rect.height = height;
  }
}

bool CmpRectTopLeft(const Rect &a, const Rect &b)
{
  if(a.y != b.y)
    return a.y < b.y;
  return a.x < b.x;
}

void SaveVideo(vector<Mat> images, string path, int fps)
{
  if(images.empty())
  {
    std::cerr<<"ERROR: Empty video\n";
    return;
  }

  cv::VideoWriter videoObject;

  videoObject.open(path, CV_FOURCC('X','V','I','D'), fps, Size(images[0].cols, images[0].rows), true);

  if(!videoObject.isOpened())
  {
    std::cerr<<"ERROR: Problem in out video path: "<<path<<"\n";
    assert(false);
  }

  for(auto img : images)
    videoObject<<img;
}








}
