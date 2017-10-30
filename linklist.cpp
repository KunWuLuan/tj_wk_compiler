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
/*返回值为-1表示识别的时候出现了错误*/
int findNumber(QTextStream &infile,c_linklist &node)
{
    char ch;
    int k=0,j=0;
    if (!infile.atEnd()) {
        infile >> ch;
        while ((ch <= '9'&&ch >= '0') || (ch == '.') || (ch == 'E') || (ch == 'e')) {
            node.m_value += ch;
            if(k==0&&(ch=='E'||ch=='e'))
                k=1;
            else if(ch=='E'||ch=='e')
                return -1;
            if(j==0&&(ch=='.'))
                j=1;
            else if(ch=='.')
                return -1;
            if (!infile.atEnd())
                infile >> ch;
            else
                break;
        }
        //将ch送回字符流中
        if (!infile.atEnd())
            infile.seek(infile.pos()-1);
    }
    if (node.m_value.indexOf('.') == -1)
        node.m_type = "CONSTINT";
    else
        node.m_type = "CONSTFLOAT";
    return 1;
}

bool isOperator(const QString &a)
{
    return !(a != "IDENTIFIER"&&a != "INT"&&a != "FLOAT"&&a != "="&&a != "NUM");
}
