#pragma once
#ifndef STRINGH_H
#define STRINGH_H
#include<string>
#endif

const int max_count = 100;
using namespace std;

int ss_test();

class SortSet {
private:
	//used表示头结点，unused表示尾结点的下一个结点
	int count=0,used=0,unused=0,tail=0;
	char ID[max_count][100];
public:
	SortSet();
	int ss_insert(const char *id);
	int ss_find(const char *id);
	int ss_delete(const char *id);
	int size();
	int next(int cur);
	int first();
	int end();
	void ss_show();
	string ss_string(int i);
};