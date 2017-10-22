#include"linklist.h"
using namespace std;
set<char>VT, VN;
string *SentencePattern;
int N;//�Ƶ���������
int getFOLLOW(const char &A, map<char, string>&F, map<char, string>&FIRST);
string getFollow(const char &A, map<char, string>&F, map<char, string>&FIRST);
int getFirst(const string &A, map<char, string>&F);
string getF(const char &A, map<char, string>&F);
string & m_erase(string &a);
int Parser()
{
	//��ȡ����ʽ���޶�VT��VN
	fstream infile;
	infile.open("data2.txt", ios::in);
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
					F[A].erase(pos);
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
				F[ch] += k.erase(t);
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
				a.erase(j,j);
				j--;
			}
		}
	}
	return a;
}