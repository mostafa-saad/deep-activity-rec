/*
 * Utilities.cpp
 *
 *  Created on: 2015-03-13
 *      Author: Moustafa S. Ibrahim
 */

#include "utilities.h"

#include <stdio.h>
#include <stdlib.h>

#include <cstring>
#include <cmath>
using std::memcpy;
using std::fabs;

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
namespace bst_fs = boost::filesystem;
using namespace boost::filesystem;

#include "custom-abbreviation.h"

namespace MostCV {

int dcmp(double x, double y) {
	return fabs(x - y) <= EPS ? 0 : x < y ? -1 : 1;
}

map<string, int> BuildStringIdMap(set<string> classes) {
	map<string, int> classId;

	REPIT(strIt, classes)
	{
		string str = *strIt;

		if (classId.count(str) == 0) {
			int sz = classId.size();
			classId[str] = sz;
		}
	}

	return classId;
}

map<string, int> BuildStringIdMap(vector<string> classesVec) {

	set<string> classes(classesVec.begin(), classesVec.end());

	return BuildStringIdMap(classes);
}

int UpdateStringIdMap(map<string, int> &classId, string str) {
	if (classId.count(str) == 0) {
		int sz = classId.size();
		classId[str] = sz;
		return sz;
	}
	return classId[str];
}

double round(double d, int precision) {
	ostringstream oss;
	oss.setf(std::ios::fixed);
	oss.precision(precision);
	oss << d;

	istringstream iss(oss.str());
	iss >> d;
	return d;
}

void fixDir(string &dir) {
	if (SZ(dir) == 0)
		return;

	if (dir[SZ(dir) - 1] != PATH_SEP)
		dir += PATH_SEP;
}

string getFileName(string dir) {
	int idx = dir.find_last_of(PATH_SEP);

	if (idx == -1)
		return dir;

	return dir.substr(idx + 1);
}

bool fileExist(string szFilePath, bool print) {
	ifstream fin(szFilePath.c_str());

	if (!fin) {
		if (print)
			printf("fileExist: Failed to open file [%s]\n", szFilePath.c_str());
		return false;
	}
	fin.close();
	return true;
}

string trim(string str) {
	int s = 0, e = SZ(str) - 1;
	REP(i, str)
	{
		if (!isspace(str[i]))
			break;
		s++;
	}

	LPD(i, SZ(str)-1, 0)
	{
		if (!isspace(str[i]))
			break;
		e--;
	}

	if (s > e)
		return "";
	return str.substr(s, e - s + 1);
}

string toLower(string str) {
	string ret = "";
	REP(i, str)
		ret += tolower(str[i]);
	return ret;
}

string toUpper(string str) {
	string ret = "";
	REP(i, str)
		ret += toupper(str[i]);
	return ret;
}

bool startsWith(string str, string pat) {
	return (int) str.find(pat) == 0;
}

int random(int range) {
	return rand() % range;
}

char* toCharArr(string str) {
	char *s = new char[SZ(str) + 1];
	s[SZ(str)] = '\0';
	memcpy(s, str.c_str(), SZ(str));
	return s;
}

string toIntStr(string st, int add, bool append_zeros) {
	int val = toType(st, 1);
	val += add;
	string ret = toString(val);

	if (append_zeros && ret.size() < st.size())
		ret = string(st.size() - ret.size(), '0') + ret;  //pad zeros
	return ret;
}

string removeExt(string name) {
	int pos = name.find_last_of('.');

	if (pos != -1)
		name = name.substr(0, pos);
	return name;
}

bool IsPathExist(string path) {
	return boost::filesystem::exists(path);
}

int CountFileLines(string path)
{
	 std::ifstream inFile(path);

	 if(inFile.fail())
	 {
		 cerr<<"Couldn't open path: "<<path<<"\n";

		 assert(false);
	 }

	 int ans = std::count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n');

	 inFile.close();

	 return ans;
}

vector<int> GetPerm(int length, int seed)
{
	boost::mt19937 randGenerator(seed);
	boost::uniform_int<> uniform_int_dist;
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rand_generator(randGenerator, uniform_int_dist);

	vector<int> perm(length);

	for (int i = 0; i < (int) perm.size(); ++i)
		perm[i] = i;

	return perm;
}

string consumeStringParam(int &argc, char** &argv, string variable_name) {
	return consumeParam(argc, argv, string(""), variable_name);
}

int consumeIntParam(int &argc, char** &argv, string variable_name) {
	return consumeParam(argc, argv, 1, variable_name);
}

double consumeDoubleParam(int &argc, char** &argv, string variable_name) {
	return consumeParam(argc, argv, 1.0, variable_name);
}

vector<string> GetDirs(string szRoot) {
	vector<string> ret;

	for (bst_fs::directory_iterator itr(szRoot); itr != bst_fs::directory_iterator(); ++itr) {

		string path_str = itr->path().c_str();

		if (bst_fs::is_directory((itr->status())))
			ret.push_back(path_str);
	}

	sort(ret.begin(), ret.end());
	return ret;
}

vector<string> GetDirsNames(string szRoot) {
	vector<string> ret;

	for (bst_fs::directory_iterator itr(szRoot); itr != bst_fs::directory_iterator(); ++itr) {

		string path_str = itr->path().c_str();

		if (bst_fs::is_directory((itr->status())))
			ret.push_back(itr->path().filename().c_str());
	}

	sort(ret.begin(), ret.end());
	return ret;
}

vector<string> GetFiles(string szRoot, string endwith) {
	vector<string> ret;

	for (bst_fs::directory_iterator itr(szRoot); itr != bst_fs::directory_iterator(); ++itr) {

		string path_str = itr->path().c_str();

		if (bst_fs::is_regular_file((itr->status())))
		{
			if(endwith == "" || boost::algorithm::ends_with(path_str, endwith))
				ret.push_back(path_str);
		}
	}

	sort(ret.begin(), ret.end());
	return ret;
}

vector<string> GetFilesExt(string szRoot, string endwith) {
	vector<string> ret;

	for (bst_fs::directory_iterator itr(szRoot); itr != bst_fs::directory_iterator(); ++itr) {
		string path_str = itr->path().c_str();

		if (bst_fs::is_regular_file((itr->status())))
		{
			if(endwith == "" || boost::algorithm::ends_with(path_str, endwith))
				ret.push_back(itr->path().extension().c_str());
		}
	}

	sort(ret.begin(), ret.end());
	return ret;
}

vector<string> GetFilesNames(string szRoot, string endwith) {
	vector<string> ret;

	for (bst_fs::directory_iterator itr(szRoot); itr != bst_fs::directory_iterator(); ++itr) {

		string path_str = itr->path().c_str();

		if (bst_fs::is_regular_file((itr->status())))
		{
			if(endwith == "" || boost::algorithm::ends_with(path_str, endwith))
				ret.push_back(itr->path().filename().c_str());
		}
	}

	sort(ret.begin(), ret.end());

	return ret;
}





























}

