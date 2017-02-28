/*
 * custom-macros.h
 *
 *  Created on: 2015-05-21
 *      Author: Moustafa S. Ibrahim
 */

#ifndef CUSTOM_MACROS_H_
#define CUSTOM_MACROS_H_

namespace MostCV {

#define ALL(v)        ((v).begin()), ((v).end())
#define RALL(v)       ((v).rbegin()), ((v).rend())
#define SZ(v)         ((int)((v).size()))
#define CLR(v, d)     memset(v, d, sizeof(v))
#define REP(i, v)     for(int i=0;i<SZ(v);++i)
#define REPI(i, j, v)     for(int i=(j);i<SZ(v);++i)
//#define REPIT(i, c) for(typeof((c).begin()) i = (c).begin(); i != (c).end(); i++)
#define REPIT(i, c) for(auto i = (c).begin(); i != (c).end(); i++)
#define LP(i, n)      for(int i=0;i<(int)(n);++i)
#define LPI(i, j, n)  for(int i=(j);i<(int)(n);++i)
#define LPD(i, j, n)    for(int i=(j);i>=(int)(n);--i)
#define REPA(v)       lpi(i, 0, SZ(v)) lpi(j, 0, SZ(v[i]))

// ToDo: http://www.quora.com/What-are-some-macros-that-are-used-in-programming-contests
}

#endif /* CUSTOM_MACROS_H_ */
