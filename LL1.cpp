#define _CRT_SECURE_NO_WARNINGS
#include"LL1.h"
#include<iostream>
#include<sstream>
const string LL1::default_path = "语法规则.txt";

string itoa(int a) {
	stringstream ss;
	ss << a;
	string tmp;
	ss >> tmp;
	return tmp;
}

LL1::LL1(string file)
{
	LL1_load(file);
}

void LL1::fir_fol_show()
{
	cout << "-------------------First---------------------" << endl;
	for (map<int, set<int>>::iterator ite = first.begin(); ite != first.end(); ite++) {
		if (ite->first<0)
			cout << VT.ss_string(-ite->first) << ':';
		else
			cout << VN.ss_string(ite->first) << ':';
		for (set<int>::iterator site = ite->second.begin(); site != ite->second.end(); site++) {
			if (*site<0)
				cout << VT.ss_string(-*site) << ' ';
			else
				cout << VT.ss_string(*site) << ' ';
		}
		cout << endl;
	}
	cout << "-------------------Follow---------------------" << endl;
	for (map<int, set<int>>::iterator ite = follow.begin(); ite != follow.end(); ite++) {
		if (ite->first<0)
			cout << VT.ss_string(-ite->first) << ':';
		else
			cout << VN.ss_string(ite->first) << ':';
		for (set<int>::iterator site = ite->second.begin(); site != ite->second.end(); site++) {
			if (*site<0)
				cout << VT.ss_string(-*site) << ' ';
			else
				cout << VT.ss_string(*site) << ' ';
		}
		cout << endl;
	}
	cout << "----------------------------------------------" << endl;
}

int LL1::LL1_load(const string file)
{
	ifstream fileloading(file.c_str());
	if (!fileloading.is_open())
		return -1;
	while (!fileloading.eof()) {
		string tmp;
		getline(fileloading, tmp, '\n');
		if (tmp.find('#') != string::npos)
			tmp = tmp.substr(0, tmp.find('#'));
		if (tmp.back() != ' '&&tmp.back() != '\n')
			tmp += '\n';
		int left = 0, right = 0;
		for (; right < tmp.length(); right++)
		{
			if (tmp[right] == ' ' || tmp[right] == '\n') {
				char str[100];
				strcpy(str, tmp.substr(left, right - left).c_str());
				if (left&&VN.ss_find(str) == -1) {
					VT.ss_insert(str);
				}
				else {
					VN.ss_insert(str);
					VT.ss_delete(str);
				}
				left = right + 1;
			}
		}
	}
	VT.ss_show();
	VN.ss_show();
	fileloading.seekg(0,ios::beg);
	int num = 0;
	while (!fileloading.eof()) {
		string tmp;
		getline(fileloading, tmp, '\n');
		if (tmp.find('#') != string::npos)
			tmp = tmp.substr(0, tmp.find('#'));
		if (tmp.back() != ' '&&tmp.back() != '\n')
			tmp += '\n';
		int left = 0, right = 0, k=0;
		for (; right < tmp.length(); right++)
		{
			if (tmp[right] == ' ' || tmp[right] == '\n') {
				char str[100];
				strcpy(str, tmp.substr(left, right - left).c_str());
				int pos = VN.ss_find(str);
				if(pos !=-1){
					exp[num] += " ";
					exp[num][k++] = pos;
				}
				else{
					exp[num] += " ";
					exp[num][k++] = -VT.ss_find(str);
				}
				left = right + 1;
			}
		}
		num++;
	}
	numOfExp = num;
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < exp[i].length(); j++) {
			cout << (int)exp[i][j] << ' ';
		}
		cout << endl;
	}
	return 0;
}

int LL1::LL1_analyse()
{
	int head = VT.first(),cur=head;
	symOfVoid = -VT.ss_find("@");
	for (; cur != VT.end(); cur = VT.next(cur)) {
		set<int>tmp;
		tmp.insert(-cur);
		first.insert(pair<int, set<int>>(-cur,tmp));
	}

	first.erase(0);
	for (cur=VN.first(); cur != VN.end(); cur = VN.next(cur)) {
		getFirst(cur,symOfVoid);
	}
	//fir_fol_show();

	int endOfAnaly = VT.ss_insert("#");
	int startOfAnaly = VN.ss_find("E");
	set<int> s;
	s.insert(-endOfAnaly);
	follow.insert(pair<int, set<int>>(startOfAnaly, s));
	for (cur = VN.first(); cur != VN.end(); cur = VN.next(cur)) {
		getFollow(cur, symOfVoid);
	}
	fir_fol_show();

	for (int i = 0; i < numOfExp; i++) {
		set<int> firstOfExp;
		set<int> followOfExp;
		for (int j = 2; j < exp[i].length(); j++) {
			set<int> f = getFirst(exp[i][j],symOfVoid);
			firstOfExp.insert(f.begin(), f.end());
			if (f.find(symOfVoid) == f.end()) {
				break;
			}
			else {
				if (j != exp[i].length() - 1) {
					firstOfExp.erase(symOfVoid);
				}
			}
		}
		//将first中的元素加入预测分析表



		//将follow中的元素加入预测分析表
		if (firstOfExp.find(symOfVoid) == firstOfExp.end()) {
			followOfExp = getFollow(exp[i][0],symOfVoid);
			for (set<int>::iterator ite = followOfExp.begin(); ite != followOfExp.end(); ite++) {
				posxy pos;
				pos.x = exp[i][0];
				pos.y = *ite;
				pTable.insert(pair<posxy, int>(pos, i));
			}
		}
	}
	return 0;
}

set<int> LL1::getFirst(int a,int T)
{
	if(first.find(a)!=first.end())
		return first.find(a)->second;
	else {
		set<int> tmp;
		for (int i = 0; i < numOfExp; i++){
			if (exp[i][0]==a) {
				for (int j = 2; j < exp[i].length();) {
					set<int> f = getFirst((int)exp[i][j], T);
					if (j == exp[i].length() - 1 && f.find(T)!= f.end()) {
						tmp.insert(f.begin(), f.end());
						break;
					}
					else if(j!=exp[i].length()-1&&f.find(T)!=f.end()){
						f.erase(T);
						tmp.insert(f.begin(), f.end());
						j++;
					}
					else if (f.find(T)==f.end()) {
						tmp.insert(f.begin(), f.end());
						break;
					}
				}
			}
		}
		first.insert(pair<int, set<int>>(a, tmp));
		return tmp;
	}
}

set<int> LL1::getFollow(int a, int T)
{
	if (follow.find(a) != follow.end()) {
		return follow.find(a)->second;
	}
	else {
		set<int>tmp;
		for (int i = 0; i < numOfExp; i++) {
			int pos = exp[i].find((char)a, 2);
			if (pos != string::npos) {
				int k = 1;
				while (k+pos < exp[i].length()) {
					set<int>t = getFirst(exp[i][pos + k], symOfVoid);
					if (k+pos==exp[i].length()-1&&t.find(symOfVoid) != t.end()) {
						tmp.insert(t.begin(), t.end());
						set<int> fol;
						if (exp[i][0] != a) {
							fol = getFollow(exp[i][0], T);
							tmp.insert(fol.begin(), fol.end());
						}
						break;
					}
					else{
						k++;
						tmp.insert(t.begin(), t.end());
						if (t.find(symOfVoid) == t.end()) {
							break;
						}
					}
				}
				if (pos == exp[i].length() - 1) {
					set<int> fol;
					if (exp[i][0] != a) {
						fol = getFollow(exp[i][0], T);
						tmp.insert(fol.begin(), fol.end());
					}
				}
			}
		}
		tmp.erase(symOfVoid);
		follow.insert(pair<int, set<int>>(a, tmp));
		return tmp;
	}
}
