#define _CRT_SECURE_NO_WARNINGS
#ifndef SORTSET_H
#define SORTSET_H
#include"SortSet.h"
#endif
#ifndef IOSTREAM_H
#define IOSTREAM_H
#include<iostream>
#endif

#define too_many_items -1
#define item_already_exist -2
#define item_no_found -1

SortSet::SortSet()
{
	for (int i = 0; i < max_count; i++) {
		memset(ID[i], 0, 100 * sizeof(char));
	}
	for (int i = 0; i < max_count; i++)
		ID[i][0] = i + 1;
}

int SortSet::ss_insert(const char *id)
{
	if (count == max_count)
		return too_many_items;
	for (int i = used; i != unused; i=ID[i][0])
		if (strcmp(id, &ID[i][1]) == 0)
			return item_already_exist;
	strcpy(&ID[unused][1], id);
	int j = unused;
	unused = ID[unused][0];
	count++;
	return j;
}

int SortSet::ss_find(const char * id)
{
	for (int i = used; i != unused; i = ID[i][0])
		if (strcmp(id, &ID[i][1]) == 0)
			return i;
	return item_no_found;
}

int SortSet::ss_delete(const char * id)
{
	int j=used,pos=-1;
	for (int i = used; i != unused; j=i,i = ID[i][0])
		if (strcmp(id, &ID[i][1]) == 0) {
			pos = i;
			ID[j][0] = ID[i][0];
			ID[i][0] = unused;
			i = j;
			count--;
		}
	if(pos == -1)
		return item_no_found;
	else
	{
		ID[j][0] = unused = pos;
		return pos;
	}
}

void SortSet::ss_show()
{
	cout << "--------------共有" << count << "项-----------------" << endl;
	for (int i = used,j=1; i != unused; i = ID[i][0],j++) {
		cout <<i<<"--"<< (int)ID[i][0] << "--" << &ID[i][1]<<'\t';
		if (j % 2 == 0)
			cout << endl;
	}
	cout <<endl<< "--------------------------------------------------" << endl;
}

int SortSet::size()
{
	return count;
}

int SortSet::next(int cur)
{
	return ID[cur][0];
}

int SortSet::first()
{
	return used;
}

int SortSet::end()
{
	return unused;
}

int ss_test()
{
	SortSet ss;
	ss.ss_insert("q");
	ss.ss_insert("w");
	ss.ss_insert("e");
	ss.ss_insert("r");
	ss.ss_show();
	cout <<"w的位置是"<< ss.ss_find("w") << endl;
	cout << "删除"<<ss.ss_delete("w")<<"位置的w"<<endl;
	ss.ss_show();
	cout << "插入g在" << ss.ss_insert("g") << "位置" << endl;
	ss.ss_show();
	return 0;
}

string SortSet::ss_string(int i){
	return string(&ID[i][1]);
}