#pragma once
#ifndef STRING_H
#define STRING_H
#include<string>
#endif
#ifndef FSTREAM_H
#define FSTREAM_H
#include<fstream>
#endif
#ifndef IOSTREAM_H
#define IOSTREAM_H
#include<iostream>
#endif
#ifndef LIST_H
#define LIST_H
#include<list>
#endif
#include<regex>
using namespace std;

enum lex_type { INT, DOUBLE, CHAR, STRING, NUMBER, VOID, CTRL, CONST, SYMBOL, IDENTIFIER, };

struct lex_node {
	lex_type type;
	string value;
};

int lex_scanner(list<lex_node> &lex, ifstream &stream);
void lex_show(list<lex_node> &lex);