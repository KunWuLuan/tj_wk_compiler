
#include"linklist.h"
STATUS c_scanner(SOURSE &infile, list<c_linklist> &Lexical);//ɨ������ÿ��ɨ���һ�������֣���ʶ��������
STATUS findNumber(ifstream &infile, c_linklist &node);
bool isOperator(const TYPE &a);
set<TYPE> str = { "and","and","eq","asm","auto",
					"bitand","bitor", "bool","break",
					"case","catch","char","class",
					"compl","const","const_cast","continue",
					"default","delete","do","double",
					"dynamic_cast","else","enum","explicit",
					"export","extern","false","float",
					"for","friend","goto","if",
					"inline","int","long","mutable",
					"namespace","new","not","not_eq",
					"operator","or","or_eq","private",
					"protected","public","register","reinterpret_cast",
					"return","short","signed","struct",
					"sizeof","static","static_cast","throw",
					"switch","template","this","typeid",
					"true","try","typedef","using",
					"typename","union","unsigned","virtual",
					"void","volatile","wchar_t","while",
					"xor","xor_eq","include"};

int main()
{
	ifstream infile;
	infile.open("test.cpp", ios::in);//���ļ�����Դ����
	if (!infile.is_open())
	{
		cout << "error" << endl;
		exit(-1);
	}
	infile >> noskipws;
	list<c_linklist>Lexical;//����Ĵʷ��������������
	STATUS ret;
	while ((ret=c_scanner(infile, Lexical)) != 2&&ret!=-1)
		;
	list<c_linklist>::iterator ite = Lexical.begin();
	//while (ite != Lexical.end())
	//{
	//	cout << '<' << (*ite).m_type << ' ' << (*ite).m_value << '>' << endl;
	//	ite++;
	//}
	if (ret == -1)
	{
		cout << "�ʷ���������" << endl;
		exit(-1);
	}
	Parser();
	return 0;
}
STATUS c_scanner(SOURSE &infile,list<c_linklist> &Lexical)
{
	//���ڴ洢ɨ�赽�Ĵ���
	c_linklist node;
	char ch;
	if (infile.peek() != EOF)
		infile >> ch;
	else
		return 2;
	//���˿ո�
	while (ch == ' '||ch =='\n'||ch =='\t')
		if (infile.peek() != EOF)
			infile >> ch;
		else
			return 2;
	node.m_value = ch;
	if (ch >= 'a'&&ch <= 'z' || ch >= 'A'&&ch <= 'Z')
	{
		//��ʶ�����߱�����
		if (infile.peek() != EOF) {
			infile >> ch;
			while (ch >= 'a'&&ch <= 'z' || ch >= 'A'&&ch <= 'Z' || ch <= '9'&&ch >= '0') {
				node.m_value += ch;
				if (infile.peek() != EOF)
					infile >> ch;
				else
					break;
			}
			//��ch�ͻ��ַ�����
			if (infile.peek() != EOF)infile.seekg(-1, ios::cur);
		}
		set<TYPE>::iterator ite;
		if ((ite = str.find(node.m_value)) != str.end())
			//�ڱ����ֱ����ҵ�
		{
			node.m_type = (*ite);
			transform(node.m_type.begin(), node.m_type.end(), node.m_type.begin(), ::toupper);
		}
		else
		{
			node.m_type = "IDENTIFIER";
		}
	}
	else if (ch <= '9'&&ch >= '0')
	{
		//��ֵ
		findNumber(infile, node);
	}
	else if (ch == '\"')
	{
		//�ַ����ͳ���
		if (infile.peek() != EOF) {
			infile >> ch;
			node.m_value += ch;
			string::iterator ite;
			while (!(ch == '\"'&&*(ite = node.m_value.end() - 1) != '\\'))
			{
				////����ת���
				//if (*(ite = node.m_value.end() - 1) != '\\')
				//	break;
				//else
				//	//����ת���
				//	//�ҵ�һ��ת���������һ��'\'ɾ������'\'����һ���ַ����뵽�����е���
				//	//node.m_value.erase(ite, ite + 1);
				//	;
				//node.m_value += ch;
				if (infile.peek() != EOF) {
					infile >> ch;
					node.m_value += ch;
				}
				else
					return -1;
			}
			node.m_type = "CHAR*";
		}
		else
			return -1;
	}
	else if (ch == '\'')
	{
		//�ַ��ͳ���
		if (infile.peek() != EOF) {
			infile >> ch;
			if (ch == '\\') {
				infile >> ch;
				node.m_value += ch;
				infile >> ch;
			}
			else {
				node.m_value += ch;
				if (infile.peek() != EOF)infile >> ch;
				else
					return -1;
			}
			node.m_type = "CHAR";
		}
		else
			return -1;
	}
	else if (ch == '/')
	{
		//�ַ��ͳ���
		if (infile.peek() != EOF) {
			infile >> ch;
			if (ch == '\\')
				//����ע�ͣ���\n
			{
				while (ch != '\n')
					if (infile.peek() != EOF)
						infile >> ch;
					else
						return 0;
			}
			else
				//��ע�ͣ���ƥ���"*/"
			{

			}
			return 0;
		}
		else
			return -1;
	}
	else if (ch=='#')
	{
		node.m_type = "#";
		Lexical.push_back(node);
		return 1;
	}
	else
	{
		//�����ַ�
		switch (ch)
		{
			case ';':
				node.m_type = ';';
				break;
			case ',':
				node.m_type = ',';
				break;
			case '(':
				node.m_type = '(';
				break;
			case ')':
				node.m_type = ')';
				break;
			case '{':
				node.m_type = '{';
				break;
			case '}':
				node.m_type = '}';
				break;
			case '[':
				node.m_type = '[';
				break;
			case ']':
				node.m_type = ']';
				break;
			case '+':
				//+|+=|++
				if (!Lexical.empty() && isOperator(Lexical.back().m_type)) {
					if (infile.peek() != EOF)
					{
						infile >> ch;
						if (ch == '=')
							node.m_value += ch;
						else if (ch == '+')
							node.m_value += ch;
						else
							//���ַ��ͻ��ַ���
							infile.seekg(-1, ios::cur);
					}
					node.m_type = node.m_value;
				}
				else {
					findNumber(infile, node);
				}
				break;
			case '-':
				//-|-=
				if (!Lexical.empty() && isOperator(Lexical.back().m_type)) {
					//��һ�����
					if (infile.peek() != EOF) {
						infile >> ch;
						if (ch == '=')
							node.m_value += ch;
						else if (ch == '-')
							node.m_value += ch;
						else
							//���ַ��ͻ��ַ���
							infile.seekg(-1, ios::cur);
					}
					else
						node.m_type = node.m_value;
				}
				else {
					findNumber(infile, node);
				}
				break;
			case '*':
				//*|*=
				if (infile.peek() != EOF) {
					infile >> ch;
					if (ch == '=')
						node.m_value += ch;
					else
						//���ַ��ͻ��ַ���
						infile.seekg(-1, ios::cur);
				}
				node.m_type = node.m_value;
				break;
			case '/':
				///|/=
				if (infile.peek() != EOF) {
					infile >> ch;
					if (ch == '=')
						node.m_value += ch;
					else
						//���ַ��ͻ��ַ���
						infile.seekg(-1, ios::cur);
				}
				node.m_type = node.m_value;
				break;
			case ':':
				//:|::
				if (infile.peek() != EOF) {
					infile >> ch;
					if (ch == ':')
						node.m_value += ch;
					else
						//���ַ��ͻ��ַ���
						infile.seekg(-1, ios::cur);
				}
				node.m_type = node.m_value;
				break;
			case '&':
				//&|&&
				if (infile.peek() != EOF) {
					infile >> ch;
					if (ch == '&')
						node.m_value += ch;
					else
						//���ַ��ͻ��ַ���
						infile.seekg(-1, ios::cur);
				}
				node.m_type = node.m_value;
				break;
			case '|':
				//||||
				if (infile.peek() != EOF) {
					infile >> ch;
					if (ch == '|')
						node.m_value += ch;
					else
						//���ַ��ͻ��ַ���
						infile.seekg(-1, ios::cur);
				}
				node.m_type = node.m_value;
				break;
			case '=':
				//=|==
				if (infile.peek() != EOF) {
					infile >> ch;
					if (ch == '=')
						node.m_value += ch;
					else
						//���ַ��ͻ��ַ���
						infile.seekg(-1, ios::cur);
				}
				node.m_type = node.m_value;
				break;
			case '>':
				//>|>>|>=
				if (infile.peek() != EOF) {
					infile >> ch;
					if (ch == '=')
						node.m_value += ch;
					else	if (ch == '>')
						node.m_value += ch;
					else
						//���ַ��ͻ��ַ���
						infile.seekg(-1, ios::cur);
				}
				node.m_type = node.m_value;
				break;
			case '<':
				//<|<<|<=
				if (!Lexical.empty()&&Lexical.back().m_type != "INCLUDE") {
					if (infile.peek() != EOF) {
						infile >> ch;
						if (ch == '=')
							node.m_value += ch;
						else	if (ch == '<')
							node.m_value += ch;
						else
							//���ַ��ͻ��ַ���
							infile.seekg(-1, ios::cur);
					}
					node.m_type = node.m_value;
				}
				else {
					if (infile.peek() != EOF) {
						infile >> ch;
						node.m_value += ch;
						string::iterator ite;
						while (ch != '>')
						{
							if (infile.peek() != EOF) {
								infile >> ch;
								node.m_value += ch;
							}
							else
								return -1;
						}
						node.m_type = "CHAR*";
					}
				}
				break;
			case '!':
				//!|!=
				if (infile.peek() != EOF) {
					infile >> ch;
					if (ch == '=')
						node.m_value += ch;
					else
						//���ַ��ͻ��ַ���
						infile.seekg(-1, ios::cur);
				}
				node.m_type = node.m_value;
				break;
		}
	}
	Lexical.push_back(node);
	return 1;
}

STATUS findNumber(ifstream &infile,c_linklist &node)
{
	char ch;
	if (infile.peek() != EOF) {
		infile >> ch;
		while (ch <= '9'&&ch >= '0' || ch == '.') {
			node.m_value += ch;
			if (infile.peek() != EOF)
				infile >> ch;
			else
				break;
		}
		//��ch�ͻ��ַ�����
		if (infile.peek() != EOF)infile.seekg(-1, ios::cur);
	}
	if (node.m_value.find('.') != -1)
		node.m_type = "FLOAT";
	else
		node.m_type = "INT";
	return 1;
}
bool isOperator(const TYPE &a)
{
	return a != "IDENTIFIER"&&a != "INT"&&a != "FLOAT"&&a != "=";
}