#pragma once
#ifndef STRING_H
#define STRING_H
#include<string>
#endif
#ifndef FSTREAM_H
#define FSTREAM_H
#include<fstream>
#endif
#include<map>
#ifndef VECTOR_H
#define VECTOR_H
#include<vector>
#endif
#ifndef SET_H
#define SET_H
#include<set>
#endif
#ifndef SORTSET_H
#define SORTSET_H
#include"SortSet.h"
#endif

using namespace std;

struct posxy {
	int x, y;
};

class LL1 {
private:
	SortSet VT, VN;
	int numOfExp;
	string exp[100];
	//char exp[100][10];
	int symOfVoid;
	map<int, set<int>> first, follow;
	map<posxy, int>pTable;
public:
	static const string default_path;
	LL1(string file=default_path);
	void fir_fol_show();
	int LL1_load(const string file);
	int LL1_analyse();
	set<int> getFirst(int a,int T);
	set<int> getFollow(int a, int T);
};