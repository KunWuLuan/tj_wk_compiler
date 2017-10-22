#pragma once
#include<fstream>
#include<sstream>
#include<iostream>
#include<string>
#include<list>
#include<set>
#include<map>
#include <algorithm>
using namespace std;
#define TYPE string
#define VALUE string
#define STATUS int
#define SOURSE ifstream
int Parser();
struct c_linklist
{
	TYPE m_type;
	VALUE m_value;
	//c_linklist * next;
};