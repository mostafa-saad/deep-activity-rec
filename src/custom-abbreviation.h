/*
 * custom-abbreviation.h
 *
 *  Created on: 2015-06-08
 *      Author: Moustafa S. Ibrahim
 */

#ifndef CUSTOM_ABBREVIATION_H_
#define CUSTOM_ABBREVIATION_H_

#include <cmath>

namespace MostCV
{
  typedef vector<int>       vi;
  typedef vector<double>    vd;
  typedef vector< vi >      vvi;
  typedef vector< vd >      vvd;
  typedef vector<string>    vs;
  typedef long long         ll;
  typedef long double       ld;
  //typedef unsigned char   uchar;

  const ll      OO = (ll)1e10;
  const double    PI  = std::acos(-1.0);
  const long double   EPS = (1e-15);

  // 4 orthogonal directions, 4 diagonal directions and last is same position
  //int DR11[9] = {1, 0, 0, -1, 1, 1, -1, -1, 0};
  //int DC11[9] = {0, 1, -1, 0, -1, 1, -1, 1, 0};

  enum DIRS_ENUM {Left, Right, Bottpm, Top};
}


#endif /* CUSTOM_ABBREVIATION_H_ */
