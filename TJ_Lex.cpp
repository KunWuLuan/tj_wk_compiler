#include"TJ_Lex.h"

int offset;
string buffer;
regex rNumber("^(\\d+)");
regex rChar("^('\\?.{1}')");
regex rComments1("^//.*");
regex rComments2B("/\\*.*");
regex rComments2E(".*\\*/");
regex rConstString("^(\".*\")");
regex rIdentify("^([a-zA-Z_]+)");
regex rSymbol("^(\\+=?|-=?|\\*=?|/=?|=|:|;|,|\\(|\\)|\\{|\\}|>=?|<=?|!=?)");
regex rCtrl("^((while)|(if)|(else)|(return)).*");
/*
从文件流中读取出一个词法结果并接在词法分析结果链表的最后
*/
int lex_scanner(list<lex_node> &lex, ifstream &text)
{
	//读取缓冲区与新结点
	lex_node new_node;

	if (text.eof())
		return 0;
	if (offset == buffer.length()) {
		offset = 0;
		text >> buffer;
	}

	smatch re;

	//预处理，删除注释
	if (regex_match(buffer, rComments1)){
		char n;
		while (1){
			if (text.eof())
				return 0;
			//text.seekg(-1,ios::cur);
			n=text.get();
			if (n == '\n') {
				offset = 0;
				text >> buffer;
				break;
			}
			text >> buffer;
		}
	}
	else if (regex_match(buffer, rComments2B)) {
		while (!regex_match(buffer, rComments2E)) {
			if (text.eof())
				return -1;
			text >> buffer;
		}
		if (text.eof())
			return 0;
		offset = 0;
		text >> buffer;
	}
	//自动机部分
	string sub = buffer.substr(offset);
	if (sub.substr(0, 3) == "int") {
		new_node.type = INT;
		new_node.value = "int";
		offset += 3;
	}
	else if (sub.substr(0, 6) == "double") {
		new_node.type = DOUBLE;
		new_node.value = "double";
		offset += 6;
	}
	else if (sub.substr(0, 4) == "char") {
		new_node.type = CHAR;
		new_node.value = "char";
		offset += 4;
	}
	else if (sub.substr(0, 4) == "void") {
		new_node.type = VOID;
		new_node.value = "void";
		offset += 4;
	}
	else if (regex_search(sub, re, rCtrl)) {
		new_node.type = CTRL;
		new_node.value = re.str(1);
		offset += re.str(1).length();
	}
	else if (regex_search(sub, re, rNumber)) {
		new_node.type = NUMBER;
		new_node.value = re.str();
		offset += re.str().length();
	}
	else if (regex_search(sub, re, rConstString)) {
		new_node.type = CONST;
		new_node.value = re.str();
		offset += re.str().length();
	}
	else if (regex_search(sub, re, rIdentify)) {
		new_node.type = IDENTIFIER;
		new_node.value = re.str();
		offset += re.str().length();
	}
	else if (regex_search(sub, re, rSymbol)) {
		new_node.type = SYMBOL;
		new_node.value = re.str();
		offset += re.str().length();
	}
	else
		return -1;
	lex.push_back(new_node);
	return 1;
}

void lex_show(list<lex_node>& lex)
{
	list<lex_node>::iterator ite;
	for (ite = lex.begin(); ite != lex.end(); ite++)
		cout << '<' << ite->type << ',' << ite->value << '>' << endl;
}
