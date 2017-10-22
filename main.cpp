
#include"linklist.h"
STATUS c_scanner(SOURSE &infile, list<c_linklist> &Lexical);//扫描器，每次扫描出一个保留字，标识符或数字
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
	infile.open("test.cpp", ios::in);//从文件读入源程序
	if (!infile.is_open())
	{
		cout << "error" << endl;
		exit(-1);
	}
	infile >> noskipws;
	list<c_linklist>Lexical;//定义的词法分析器结果链表
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
		cout << "词法分析错误" << endl;
		exit(-1);
	}
	Parser();
	return 0;
}
STATUS c_scanner(SOURSE &infile,list<c_linklist> &Lexical)
{
	//用于存储扫描到的词语
	c_linklist node;
	char ch;
	if (infile.peek() != EOF)
		infile >> ch;
	else
		return 2;
	//过滤空格
	while (ch == ' '||ch =='\n'||ch =='\t')
		if (infile.peek() != EOF)
			infile >> ch;
		else
			return 2;
	node.m_value = ch;
	if (ch >= 'a'&&ch <= 'z' || ch >= 'A'&&ch <= 'Z')
	{
		//标识符或者保留字
		if (infile.peek() != EOF) {
			infile >> ch;
			while (ch >= 'a'&&ch <= 'z' || ch >= 'A'&&ch <= 'Z' || ch <= '9'&&ch >= '0') {
				node.m_value += ch;
				if (infile.peek() != EOF)
					infile >> ch;
				else
					break;
			}
			//将ch送回字符流中
			if (infile.peek() != EOF)infile.seekg(-1, ios::cur);
		}
		set<TYPE>::iterator ite;
		if ((ite = str.find(node.m_value)) != str.end())
			//在保留字表中找到
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
		//数值
		findNumber(infile, node);
	}
	else if (ch == '\"')
	{
		//字符串型常量
		if (infile.peek() != EOF) {
			infile >> ch;
			node.m_value += ch;
			string::iterator ite;
			while (!(ch == '\"'&&*(ite = node.m_value.end() - 1) != '\\'))
			{
				////考虑转义符
				//if (*(ite = node.m_value.end() - 1) != '\\')
				//	break;
				//else
				//	//处理转义符
				//	//找到一个转义符，将第一个'\'删掉，将'\'后面一个字符加入到串序列当中
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
		//字符型常量
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
		//字符型常量
		if (infile.peek() != EOF) {
			infile >> ch;
			if (ch == '\\')
				//单行注释，找\n
			{
				while (ch != '\n')
					if (infile.peek() != EOF)
						infile >> ch;
					else
						return 0;
			}
			else
				//段注释，找匹配的"*/"
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
		//其他字符
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
							//将字符送回字符流
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
					//是一个算符
					if (infile.peek() != EOF) {
						infile >> ch;
						if (ch == '=')
							node.m_value += ch;
						else if (ch == '-')
							node.m_value += ch;
						else
							//将字符送回字符流
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
						//将字符送回字符流
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
						//将字符送回字符流
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
						//将字符送回字符流
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
						//将字符送回字符流
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
						//将字符送回字符流
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
						//将字符送回字符流
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
						//将字符送回字符流
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
							//将字符送回字符流
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
						//将字符送回字符流
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
		//将ch送回字符流中
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