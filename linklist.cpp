#include"linklist.h"

c_linklist::c_linklist()
{
	m_type = m_value = "";
}
c_linklist::c_linklist(string a,string b)
{
	m_type = a;
	m_value = b;
}