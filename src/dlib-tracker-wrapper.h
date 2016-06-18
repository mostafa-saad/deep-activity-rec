/*
 * dlib-tracker-wrapper.h
 *
 *  Created on: 2015-06-22
 *      Author: Moustafa S. Ibrahim
 */

#ifndef DLIB_TRACKER_WRAPPER_H_
#define DLIB_TRACKER_WRAPPER_H_

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using cv::Mat;
using cv::Ptr;
using cv::Scalar;
using cv::Rect;
using cv::Point;
using cv::Size;

#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/dir_nav.h>

#include <vector>
using std::vector;
using std::pair;

namespace MostCV {

class DlibTrackerWrapper {
public:
  DlibTrackerWrapper(Rect initial_location);

  Rect UpdateTracker(Mat img);
  pair<vector<Mat>, vector<Rect> > Process(vector<Mat> backwardImgs, vector<Mat> forwardImgs);

private:
  dlib::correlation_tracker tracker_;
  Rect initial_location_;
  int step_;
};


}

#endif /* DLIB_TRACKER_WRAPPER_H_ */
