/*
 * general_utilities.h
 *
 *  Created on: 2015-03-11
 *      Author: Moustafa S. Ibrahim
 */

#ifndef GENERAL_UTILITIES_H_
#define GENERAL_UTILITIES_H_

#include "custom-macros.h"

#include <assert.h>
#include<string>
#include<vector>
#include<map>
#include<set>
#include<iostream>
#include<sstream>
#include<fstream>

using std::string;
using std::ostringstream;
using std::istringstream;
using std::ifstream;
using std::set;
using std::map;
using std::vector;
using std::cout;
using std::cerr;
using std::pair;

namespace MostCV {

const char PATH_SEP = '/';

int dcmp(double x, double y);

double round(double d, int precision);

void fixDir(string &dir);

bool IsPathExist(string path);

string getFileName(string dir);

bool fileExist(string szFilePath, bool print = true);

string trim(string str);

string toLower(string str);

string toUpper(string str);

bool startsWith(string str, string pat);

int random(int range);

char* toCharArr(string str);

string toIntStr(string st, int add, bool append_zeros = true);

string removeExt(string name);

map<string, int> BuildStringIdMap(set<string> classId);

map<string, int> BuildStringIdMap(vector<string> classesVec);

int UpdateStringIdMap(map<string, int> &items_map, string str);

int CountFileLines(string path);

vector<int> GetPerm(int length, int seed = 123);

string consumeStringParam(int &argc, char** &argv, string variable_name = "");
int consumeIntParam(int &argc, char** &argv, string variable_name = "");
double consumeDoubleParam(int &argc, char** &argv, string variable_name = "");

vector<string> GetDirs(string szRoot);
vector<string> GetDirsNames(string szRoot);
vector<string> GetFiles(string szRoot, string endwith = "");
vector<string> GetFilesExt(string szRoot, string endwith = "");
vector<string> GetFilesNames(string szRoot, string endwith = "");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Type> Type toType(string data, Type indicator, string variable_name = "") {
  istringstream iss(data);

  Type item;
  iss >> item;

  if(iss.fail())
  {
    if(variable_name != "")
      cerr<<"Problem in reading variable: "<<variable_name<<"\n";
    cerr<<"Failed to convert string: ["<<data<<"] to same type as variable ["<<indicator<<"]\n";
    assert(false);
  }

  return item;
}

template<class Type> Type consumeParam(int &argc, char** &argv, Type indicator, string variable_name = "")
{
  assert(argc > 0);
  string ret = argv[0];
  --argc, ++argv;
  return toType(ret, indicator, variable_name);
}



template<class Type> char* toCharPtr(Type val) {
  ostringstream oss;
  oss << val;
  return toCharArr(oss.str());
}

template<class Type> string toString(Type val) {
  ostringstream oss;
  oss << val;
  return oss.str();
}

template<class Type> vector<Type> readStringItems(string data, Type indicator) {
  vector<Type> items;
  Type item;

  istringstream iss(data);

  while (iss >> item)
    items.push_back(item);

  return items;
}

template<class Type> vector<Type> readFileItems(string filePath, Type indicator, bool print = true) {
  vector<Type> items;
  Type item;

  ifstream fin(filePath.c_str());
  if (!fin) {
    if (print)
      printf("\n\tWARNING: readFileItems: Failed to open file [%s]\n", filePath.c_str());
    fflush(stdout);
    return items;
  }

  while (fin >> item)
    items.push_back(item);

  fin.close();
  return items;
}

template<class Type> vector<Type> readFileItems(ifstream &fin, Type indicator, int length = -1) {
  Type item;
  vector<Type> items;

  if(length == -1)
  {
	  while (items.sizefin >> item)
	      items.push_back(item);
  }
  else
  {
	  items.resize(length);

	  for (int pos = 0; pos < items.size(); ++pos)
	  {
		  fin >> item;

		  assert(!fin.fail());

		  items[pos] = item;
	  }
  }


  return items;
}

template<class Type> vector<Type> readStreamItems(istringstream &iss, Type indicator, int length = -1) {
  Type item;
  vector<Type> items;

  if(length == -1)
  {
	  while (iss >> item)
	      items.push_back(item);
  }
  else
  {
	  items.resize(length);

	  for (int pos = 0; pos < items.size(); ++pos)
	  {
		  iss >> item;

		  assert(!iss.fail());

		  items[pos] = item;
	  }
  }


  return items;
}


template<class Type> vector<vector<Type> > read2dFileItems(string filePath, Type indicator, bool print = true) {
  vector<vector<Type> > items;

  ifstream fin(filePath.c_str());

  if (fin.fail()) {
      printf("read2dFileItems: Failed to open file [%s]\n", filePath.c_str());
      assert(false);
  }

  string line;
  while (getline(fin, line))
  {
	  if(line != "")
		  items.push_back(readStringItems(line, indicator));
  }

  return items;
}



// For every element that has max frequency, add its position. Total elements equal to # of unqiue elements
// 2 3 2 2 2 2 4 4    => 0 6 1
template<class Type> vector<int> getMaxFrequentPositions(vector<Type> &vec) {
  vector<int> retVec;
  map<Type, vector<int> > freq_map;

  for (int i = 0; i < vec.size(); ++i)
    freq_map[vec[i]].push_back(i);

  set<pair<int, vector<int> >, std::greater<pair<int, vector<int> > > > freqs;

  for (auto kv : freq_map)
    freqs.insert(std::make_pair(kv.second.size(), kv.second));

  for (auto group : freqs)
    retVec.push_back(group.second[0]);

  return retVec;
}

template<class Type> Type getMaxFrequentLabel(vector<Type> &vec)
{
  assert(vec.size() > 0);

  vector<int> pos = getMaxFrequentPositions(vec);

  return vec[ pos[0] ];
}

template<class Type> map<Type, int> getFrequencyMap(const vector<Type> &vec, bool print = false) {
  map<Type, int> freq_map;

  for (int i = 0; i < vec.size(); ++i)
    freq_map[vec[i]]++;

  if (print) {
    for (auto kv : freq_map)
      cerr << "Key = "<<kv.first << "\t => Value " << kv.second << " instances\n";
  }

  return freq_map;
}

template<class Type> map<Type, int> getFrequencyMapPercent(vector<Type> &vec, bool print = false) {
  map<Type, int> freq_map;

  for (int i = 0; i < vec.size(); ++i)
    freq_map[vec[i]]++;

  if (print) {
    cerr.precision(1);
    cerr.setf(std::ios::fixed);

    for (auto kv : freq_map)
      cerr << "Key = "<<kv.first << "\t => Value " << 100.0 * kv.second / (double)vec.size()<< " %\n";
  }

  return freq_map;
}

template<class Type1, class Type2> vector<Type2> castVector(const vector<Type1> &row, Type2 indicator) {
  vector<Type2> ret;

  ret.reserve(row.size());

  for(auto val : row)
    ret.push_back((Type2)val);

  return ret;
}

template<class Type1, class Type2> vector<vector<Type2>> cast2DVector(const vector<vector<Type1>> &matrix, Type2 indicator) {
  vector<vector<Type2>> ret;

  ret.reserve(matrix.size());

  for(auto row : matrix)
    ret.push_back(castVector(row, indicator));

  return ret;
}


}

#endif /* GENERAL_UTILITIES_H_ */
