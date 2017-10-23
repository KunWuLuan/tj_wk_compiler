#include"linklist.h"
using namespace std;
string keyname[] = {
	"INT","VOID","IDENTIFIER","RETURN","WHILE","ELSE","NUM","IF"
};
set<char>VT, VN;
string *SentencePattern;
int N;//�Ƶ���������
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
	//��ȡ����ʽ���޶�VT��VN
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
	cout << "���ս��: ";
	for (ite = VT.begin(); ite != VT.end(); ite++)
		cout << *ite << ' ';
	cout << endl;
	cout << "�ս��: ";
	for (ite = VN.begin(); ite != VN.end(); ite++)
		cout << *ite << ' ';
	cout << endl;
	//������ݹ飬Ȼ����ȡ��������
	//�Ѿ����,����Ҫ��ʾ���в���
	//����ÿ�����ŵ�FIRST���ϡ�FOLLOW����
	//FIRST����
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
	//FOLLOW����
	map<char, string>FOLLOW;
	FOLLOW[SentencePattern[0][0]] = '#';
	for (int i = 0; i < N; i++)
	{
		char k = SentencePattern[i][0];
		getFOLLOW(k, FOLLOW, FIRST);
		FOLLOW[k] = m_erase(FOLLOW[k]);
	}
	//����Ԥ�������
	map<string, string>Forecast;
	stringstream sstr;
	for (int i = 0; i < N; i++)
	{
		string sentences = SentencePattern[i];
		int firpos = 2,secpos;
		secpos = sentences.find('|');
		for (int i=firpos; i < sentences.length(); i++)
		{
			char ch = sentences[i];//����ÿ������FIRST�����ս��
			for (int j = 0; j < FIRST[ch].length(); j++)//�ڶ���
			{
				string key;
				if (FIRST[ch][j] == '@')
					continue;
				sstr<< sentences[0] << FIRST[ch][j];//�����ֵAa
				sstr >> key;
				sstr.clear();
				int len = (secpos == string::npos) ? sentences.length() - firpos : secpos - firpos + 1;
				Forecast[key] += sentences.substr(firpos, len);//������ʽ���뵽M(A,a)
			}
			if (FIRST[ch].find('@') != string::npos)//�˲���ʽ�Ĳ��ֵ��׷�����û������
			{
				if (i + 1 == sentences.length() || sentences[i + 1] == '|')//˵��FIRST[alpha]�����գ����ǵ�����
				{
					for (int j = 0; j < FOLLOW[sentences[0]].length(); j++)
					{
						string key;
						sstr << sentences[0] << FOLLOW[sentences[0]][j];//�����ֵAb
						sstr >> key;
						sstr.clear();
						int len = (secpos == string::npos) ? sentences.length() - firpos : secpos - firpos + 1;
						Forecast[key] += sentences.substr(firpos, len);//������ʽ���뵽M(A,b)
					}
				}
				continue;
			}
			else//������һ������ʽ
			{
				if (secpos == string::npos)//��һ���Ѿ���������
				{
					break;
				}
				firpos = secpos + 1;
				i = secpos;//������i��Ҫ++�����Բ���ֵfirpos
				secpos = sentences.find('|', secpos + 1);
			}
		}
	}

	//����Ԥ�������ʹʷ�������������﷨����
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
				//���в���ʽB����A��,
				//��FIRST(��) - {��}�ӵ�FOLLOW(A)��;
				//��B ����A��, �Ҧ¿����Ƶ�����
				//��FOLLOW(B)�ӵ�FOLLOW(A)�У�
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
				//B����A
				F[A] += getFollow(a[0], F, FIRST);
			}
		}
	}
	return 1;
}

string getFollow(const char &A, map<char, string>&F, map<char, string>&FIRST)//�˺�����getF��������������һ���ݹ顣
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
			if (ite == string::npos)//���Ѿ�������
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
		string posxy;//��������
		TYPE k = (*ite).m_type;
		int pos = findtype(k);//�ҵ������͵ı��
		char top = symbol.top();//ȡ��ջ���ķ���
		if (top == '|') {
			symbol.pop();
			continue;
		}
		if (pos == -1)//�Ǹ�����
		{
			if (k[0] == top)
			{
				//��һ���ս������ƥ��ɹ�
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
				//��һ���ս������ƥ��ɹ�
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
		else//���Ǹ����ţ��ñ�Ž���ƥ��
		{
			sstr << top << pos;
			sstr >> posxy;
		}
		if (pos == top - '0')
		{
			//��һ���ս������ƥ��ɹ�
			symbol.pop();
			cout << (*ite).m_value;
			ite++;
			hang++;
			cout << endl<<'\t';
			continue;
		}
		cout << symbol.top() << '\t';

		string pattern;
		if (Forecast.count(posxy) == 0)//������Ԥ�������֮������������
			return -1;
		pattern = Forecast[posxy];//�鵽����ʽ
		symbol.pop();
		if (pattern == "@" || pattern == "@|") {
			cout << "@" << endl << '\t';
			continue;
		}
		for (int i = pattern.length() - 1; i >= 0; i--)
		{
			symbol.push(pattern[i]);//���򽫲���ʽѹջ
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