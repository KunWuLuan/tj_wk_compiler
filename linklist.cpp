#include"tj_lexical.h"

c_linklist::c_linklist()
{
    m_type = m_value = "";
}
c_linklist::c_linklist(QString a,QString b)
{
    m_type = a;
    m_value = b;
}

STATUS findNumber(QTextStream &infile,c_linklist &node)
{
    char ch;
    if (!infile.atEnd()) {
        infile >> ch;
        while (ch <= '9'&&ch >= '0' || ch == '.') {
            node.m_value += ch;
            if (!infile.atEnd())
                infile >> ch;
            else
                break;
        }
        //将ch送回字符流中
        if (!infile.atEnd())
            infile.seek(infile.pos()-1);
    }
    if (node.m_value.indexOf('.') != -1)
        node.m_type = "NUM";
    else
        node.m_type = "NUM";
    return 1;
}

bool isOperator(const TYPE &a)
{
    return !(a != "IDENTIFIER"&&a != "INT"&&a != "FLOAT"&&a != "="&&a != "NUM");
}
