#ifndef STRING_H
#define STRING_H
#include<string>
#endif
#ifndef FSTREAM_H
#define FSTREAM_H
#include<fstream>
#endif
#include<stdio.h>
#ifndef IOSTREAM_H
#define IOSTREAM_H
#include<iostream>
#endif
#include<iomanip>
#include<time.h>
using namespace std;
class logfile{
public:
    static int create_log(fstream &log, bool tty= true);
	static void writelog(fstream &log, void* msg,int len);
};
