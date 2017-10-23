#include"linklist.h"
using namespace std;
string keyname[] = {
	"INT","VOID","IDENTIFIER","RETURN","WHILE","ELSE","NUM","IF"
};
set<char>VT, VN;
string *SentencePattern;
int N;//推导语句的数量
int getFOLLOW(const char &A, map<char, string>&F, map<char, string>&FIRST);
string getFollow(const char &A, map<char, string>&F, map<char, string>&FIRST);
int getFirst(const string &A, map<char, string>&F);
string getF(const char &A, map<char, string>&F);
bool isRELOP(string k);
string & m_erase(string &a);
STATUS analyse(list<c_linklist> Lexical,map<string,string>Forecast);
int findtype(string k);
int Parser(list<c_linklist> Lexical)
{
	//读取递推式，限定VT和VN
	fstream infile;
	infile.open("data4.txt", ios::in);
	if (!infile.is_open())
		return -1;
	infile >> N;
	SentencePattern = new string[N];
	for (int i = 0; i < N; i++)
	{
		infile >> SentencePattern[i];
		VT.insert(SentencePattern[i][0]);
		for (int j = 2; j < SentencePattern[i].length(); j++)
		{
			VN.insert(SentencePattern[i][j]);
		}
	}
	VN.erase('|');
	set<char>::iterator ite;
	for (ite = VT.begin(); ite != VT.end(); ite++)
		VN.erase(*ite);
	cout << "非终结符: ";
	for (ite = VT.begin(); ite != VT.end(); ite++)
		cout << *ite << ' ';
	cout << endl;
	cout << "终结符: ";
	for (ite = VN.begin(); ite != VN.end(); ite++)
		cout << *ite << ' ';
	cout << endl;
	//消除左递归，然后提取公共因子
	//已经完成,不需要显示进行操作
	//产生每个符号的FIRST集合、FOLLOW集合
	//FIRST集合
	map<char, string>FIRST;
	for (ite = VN.begin(); ite != VN.end(); ite++)
	{
		char ch = *ite;
		FIRST[ch] = ch;
	}
	for (int i = 0; i < N; i++)
	{
		getFirst(SentencePattern[i], FIRST);
	}
	for (ite = VT.begin(); ite != VT.end(); ite++)
	{
		FIRST[*ite] = m_erase(FIRST[*ite]);
	}
	//FOLLOW集合
	map<char, string>FOLLOW;
	FOLLOW[SentencePattern[0][0]] = '#';
	for (int i = 0; i < N; i++)
	{
		char k = SentencePattern[i][0];
		getFOLLOW(k, FOLLOW, FIRST);
		FOLLOW[k] = m_erase(FOLLOW[k]);
	}
	//产生预测分析表
	map<string, string>Forecast;
	stringstream sstr;
	for (int i = 0; i < N; i++)
	{
		string sentences = SentencePattern[i];
		int firpos = 2,secpos;
		secpos = sentences.find('|');
		for (int i=firpos; i < sentences.length(); i++)
		{
			char ch = sentences[i];//对于每个属于FIRST集的终结符
			for (int j = 0; j < FIRST[ch].length(); j++)//第二部
			{
				string key;
				if (FIRST[ch][j] == '@')
					continue;
				sstr<< sentences[0] << FIRST[ch][j];//构造键值Aa
				sstr >> key;
				sstr.clear();
				int len = (secpos == string::npos) ? sentences.length() - firpos : secpos - firpos + 1;
				Forecast[key] += sentences.substr(firpos, len);//将产生式加入到M(A,a)
			}
			if (FIRST[ch].find('@') != string::npos)//此产生式的部分的首符集还没处理完
			{
				if (i + 1 == sentences.length() || sentences[i + 1] == '|')//说明FIRST[alpha]包含空，即是第三步
				{
					for (int j = 0; j < FOLLOW[sentences[0]].length(); j++)
					{
						string key;
						sstr << sentences[0] << FOLLOW[sentences[0]][j];//构造键值Ab
						sstr >> key;
						sstr.clear();
						int len = (secpos == string::npos) ? sentences.length() - firpos : secpos - firpos + 1;
						Forecast[key] += sentences.substr(firpos, len);//将产生式加入到M(A,b)
					}
				}
				continue;
			}
			else//处理下一个产生式
			{
				if (secpos == string::npos)//第一行已经处理完了
				{
					break;
				}
				firpos = secpos + 1;
				i = secpos;//接下来i还要++，所以不赋值firpos
				secpos = sentences.find('|', secpos + 1);
			}
		}
	}

	//根据预测分析表和词法分析结果进行语法分析
	analyse(Lexical, Forecast);
	return 0;
}

int getFOLLOW(const char &A, map<char, string>&F, map<char, string>&FIRST)
{
	for (int i = 0; i < N; i++)
	{
		int pos;
		string a = SentencePattern[i];
		if ((pos = a.find(A, 2)) != string::npos)
		{
			if (!(a[pos + 1] == '|' || pos + 1 == a.length()))
			{
				//若有产生式B→αAβ,
				//则将FIRST(β) - {ε}加到FOLLOW(A)中;
				//若B →αAβ, 且β可以推导出ε
				//则将FOLLOW(B)加到FOLLOW(A)中；
				string b = FIRST[a[pos + 1]];
				F[A] += b;
				int pos;
				if((pos = F[A].find('@'))!=string::npos)
					F[A].erase(pos,1);
				if (b.find('@') != string::npos)
				{
					string c = getFollow(SentencePattern[i][0], F, FIRST);
					F[A] += c;
				}

			}
			else
			{
				//B→αA
				F[A] += getFollow(a[0], F, FIRST);
			}
		}
	}
	return 1;
}

string getFollow(const char &A, map<char, string>&F, map<char, string>&FIRST)//此函数和getF函数是用来产生一个递归。
{
	if (F[A] != "")
		return F[A];
	else
	{
		getFOLLOW(A, F, FIRST);
		return F[A];
	}
}

int getFirst(const string &A, map<char, string>&F)
{
	int ite;
	int num = 0;
	char ch = A[0];
	if (F[ch] != "")
		return 1;
	for (ite = 2; ite < A.length(); ite++)
	{
		string k = getF(A[ite], F);
		if (k.find('@') != string::npos)
		{
			int t = k.find('@');
			if (A[ite] == '@')
				F[ch] += '@';
			else
				F[ch] += k.erase(t, 1);
			//F[ch].erase('@');
			//num++;
		}
		else
		{
			F[ch] += k;
			int i = A.find('|', ite);
			if (ite == i)F[ch] += '@';
			ite = i;
			if (ite == string::npos)//就已经找完了
				break;
		}
	}
	//if (num == A.length() - 2)
	//	F[ch] += "@";
	return 1;
}

string getF(const char &A, map<char, string>&F)
{
	string str;
	if (A == '|')
		return "";
	if (F[A] == "")
	{
		for (int i = 0; i < N; i++)
			if (SentencePattern[i][0] == A)
				str = SentencePattern[i];
		getFirst(str, F);
		return F[A];
	}
	else
	{
		return F[A];
	}
}

string &m_erase(string &a)
{
	for (int i = 0; i < a.length(); i++)
	{
		for (int j = i + 1; j < a.length(); j++)
		{
			if (a[j] == a[i])
			{
				a.erase(j, 1);
				j--;
			}
		}
	}
	return a;
}

STATUS analyse(list<c_linklist> Lexical, map<string, string>Forecast)
{
	list<c_linklist>::iterator ite;
	stack<char>symbol;
	symbol.push('#');
	symbol.push('E');
	int hang = 0;
	for (ite = Lexical.begin(); ite != Lexical.end(); )
	{
		stringstream sstr;
		string posxy;//构建坐标
		TYPE k = (*ite).m_type;
		int pos = findtype(k);//找到该类型的编号
		char top = symbol.top();//取出栈顶的符号
		if (top == '|') {
			symbol.pop();
			continue;
		}
		if (pos == -1)//是个符号
		{
			if (k[0] == top)
			{
				//是一个终结符而且匹配成功
				symbol.pop();
				cout << (*ite).m_value;
				ite++;
				hang++;
				cout << endl<<'\t';
				//for (int j = 0; j < hang; j++)
				//	cout << '\t';
				continue;
			}
			if ('g' == top && isRELOP(k))
			{
				//是一个终结符而且匹配成功
				symbol.pop();
				cout << (*ite).m_value;
				ite++;
				hang++;
				cout << endl << '\t';
				//for (int j = 0; j < hang; j++)
				//	cout << '\t';
				continue;
			}
			if(isRELOP(k))
				sstr << top << 'g';
			else
				sstr << top << k[0];
			sstr >> posxy;
		}
		else//不是个符号，用编号进行匹配
		{
			sstr << top << pos;
			sstr >> posxy;
		}
		if (pos == top - '0')
		{
			//是一个终结符而且匹配成功
			symbol.pop();
			cout << (*ite).m_value;
			ite++;
			hang++;
			cout << endl<<'\t';
			continue;
		}
		cout << symbol.top() << '\t';

		string pattern;
		if (Forecast.count(posxy) == 0)//出现了预测分析表之外的情况，报错
			return -1;
		pattern = Forecast[posxy];//查到产生式
		symbol.pop();
		if (pattern == "@" || pattern == "@|") {
			cout << "@" << endl << '\t';
			continue;
		}
		for (int i = pattern.length() - 1; i >= 0; i--)
		{
			symbol.push(pattern[i]);//逆序将产生式压栈
		}
	}
	return 1;
}

int findtype(string k)
{
	for (int i = 0; i < 8; i++)
	{
		if (keyname[i] == k)
			return i;
	}
	return -1;
}

bool isRELOP(string k)
{
	if (k == "<" || k == "<=" || k == ">" || k == ">=" || k == "==" || k == "!=")
		return 1;
	return 0;
}