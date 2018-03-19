#include"LL1.h"
#include"TJ_Lex.h"
#include"TJ_Log.h"
const char code_test_text[] = "test.cpp";

void lex_show_file(list<lex_node>& lex,fstream &file)
{
	list<lex_node>::iterator ite;
	for (ite = lex.begin(); ite != lex.end(); ite++)
		file << '<' << ite->type << ',' << ite->value << '>' << endl;
}
int main()
{
	LL1 a;
	a.LL1_analyse();
	return 0;

	bool is_end=false;
	list<lex_node>lex_list;
	fstream test_log;
	ifstream code_stream(code_test_text);

	logfile::create_log(test_log);
	while (!is_end)
	{
		//词法分析
		int flag = lex_scanner(lex_list, code_stream);
		if (flag == 0)
			is_end = true;
		else if (flag == -1)
			goto Err1;
		//cout<< flag <<endl;
		//语法分析

		//语义分析与中间代码生成

		//目标代码生成

	}
	lex_show(lex_list);
	lex_show_file(lex_list,test_log);
	system("pause");
	return 0;
Err1:
	lex_show(lex_list);
	lex_show_file(lex_list, test_log);
	cout << "lex error" << endl;
	system("pause");
	return -1;
}