/*
 * dlib-tracker-wrapper.cpp
 *
 *  Created on: 2015-06-22
 *      Author: Moustafa S. Ibrahim
 */

#include "dlib-tracker-wrapper.h"
#include "custom-images-macros.h"

#include <iostream>
using std::cerr;

namespace MostCV {

DlibTrackerWrapper::DlibTrackerWrapper(Rect initial_location) {
  initial_location_ = initial_location;
  step_ = 0;
}

Rect DlibTrackerWrapper::UpdateTracker(Mat img) {
  Rect img_rect = Rect(0, 0, img.cols-1, img.rows-1);
  cv::Mat gray_img;

  if (CV_8U != img.type() || 1 != img.channels())
    cv::cvtColor(img, gray_img, cv::COLOR_BGR2GRAY);
  else
    gray_img = img;

  dlib::array2d<uchar> dlib_img(gray_img.rows, gray_img.cols);

  REPIMG2(y, x, gray_img)
      dlib_img[y][x] = gray_img.at<uchar> (y, x);

  if (step_ == 0) {
    initial_location_ &= img_rect;  // Fix first one in case

    if(initial_location_.area() == 0)
    {
      cerr<<"Dlib: Empty rectangle for tracking! Let's do workaround\n";

      initial_location_ = Rect(0, 0, 1, 1);
    }

    tracker_.start_track(dlib_img, dlib::centered_rect(dlib::point(initial_location_.x + initial_location_.width / 2, initial_location_.y + initial_location_.height / 2),
                                                 initial_location_.width, initial_location_.height));
    ++step_;
    return initial_location_;
  }

  tracker_.update(dlib_img);
  int y1 = tracker_.get_position().top();
  int x1 = tracker_.get_position().left();
  int y2 = tracker_.get_position().bottom();
  int x2 = tracker_.get_position().right();

  ++step_;

  Rect rect = Rect(x1, y1, x2-x1, y2-y1);


  rect &= img_rect;

  if(rect.area() < 1)   // zero areas usually cause problems. Let's give them 1 area box
    rect = Rect(0, 0, 1, 1);

  return rect;
}

// back like: 0 -1 -2 -3  and forward 0 1 2 3 4 5 6. Helps when tracker centered on frame
pair<vector<Mat>, vector<Rect> > DlibTrackerWrapper::Process(vector<Mat> backwardImgs, vector<Mat> forwardImgs)
{
  vector<Rect> ret;

  DlibTrackerWrapper backTracker(initial_location_);

  for(auto img: backwardImgs)
    ret.push_back( backTracker.UpdateTracker(img) );

  if(forwardImgs.size() > 0)
  {
    std::reverse(ret.begin(), ret.end());
    std::reverse(backwardImgs.begin(), backwardImgs.end());
    backwardImgs.pop_back();
    ret.pop_back(); // remove the middle, it will be added again. This is initial_location_
  }

  for(auto img: forwardImgs)
  {
    ret.push_back( UpdateTracker(img) );
    backwardImgs.push_back(img);
  }

  return std::make_pair(backwardImgs, ret);
}

}
