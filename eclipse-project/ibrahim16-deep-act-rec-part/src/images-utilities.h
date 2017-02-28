/*
 * ImagesHelper.h
 *
 *  Created on: 2015-03-01
 *      Author: mostafa
 */

#ifndef IMAGESHELPER_H_
#define IMAGESHELPER_H_

#include<string>
#include<vector>
using std::vector;
using std::string;

#include "opencv2/core/core.hpp"
using cv::Mat;
using cv::Ptr;
using cv::Point;
using cv::Rect;
using cv::Scalar;
using cv::Size;

#include "custom-images-macros.h"

namespace MostCV {

struct CComponenets {
  Mat mask;
  int mask_pixels_cnt;
  Rect rect;
  Point flood_starting_point;
  Point parent_mask_topleft_point;
};

void ShowImage(Mat image, int wait = 0, bool bShow = true, string stringWindowName = "Image");

void RemoveImagePixels(Mat img, Mat mask, bool is_mask_remove_pixel_black = false, Point shift = Point(0, 0));

void FixMask(Mat mask, int threshold = 10);

void Morphology(Mat mask, bool do_open = true, bool do_close = true, int open_kernel_sz = 3, int close_kernel_sz = 15);

vector<Ptr<CComponenets> > GetConnectedComponenets(Mat img, int area_threshold = 1, int pixels_threshold = 1, Scalar lo_diff = Scalar(1), Scalar up_diff =
    Scalar(1), int flags = 4 + (255 << 8));

Rect GetRect(Mat img);

Rect GetInternalBlobRect(Mat mask);

void CenterRect(Rect &target_rect, int width, int height);

vector<Point> GetCombinedContour(Mat mask);

bool AddButton(Mat controlsMat, string buttonName, vector<Rect> &rectsSoFar, Scalar color = Scalar(255, 0, 0));

bool CmpRectTopLeft(const Rect &a, const Rect &b);

void SaveVideo(vector<Mat> images, string path, int fps = 25);

////////////////////////////

template<class Type>  Mat ToRowMat(const vector<Type> &row)
{
  if(row.size() == 0)
    return Mat(0, 0, cv::DataType<Type>::type);

  const Type *ptr = &row[0];
  Mat mat = Mat(1, row.size(), cv::DataType<Type>::type);

  memcpy(mat.data, ptr, row.size()*sizeof(Type));

  //Mat tempMat = Mat(featureVec).t();

  return mat;
}

template<class Type>  Mat ToColMat(const vector<Type> &col)
{
  if(col.size() == 0)
    return Mat(0, 0, cv::DataType<Type>::type);

  const Type *ptr = &col[0];
  Mat mat = Mat(col.size(), 1, cv::DataType<Type>::type);

  memcpy(mat.data, ptr, col.size()*sizeof(Type));

  return mat;
}

template<class Type>  Mat To2DMat(const vector<vector<Type>> & vectors)
{
  Mat mat;

  for(auto row : vectors)
    mat.push_back(ToRowMat(row));

  return mat;
}

/*
template<typename function> void perform(function operation, Mat mat) {
  if(mat.channels() == 2)
  {
    REPIMG2(y, x, mat)
        mat.at<>
  }
  else {

  }
}
*/

}

#endif /* IMAGESHELPER_H_ */
