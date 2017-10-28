#include "tj_lexical.h"
#include "ui_tj_lexical.h"
#include <windows.h>
#include "main.h"
set<TYPE> conststr = { "and","and","eq","asm","auto",
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
tj_lexical::tj_lexical(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tj_lexical)
{
    ui->setupUi(this);
    data = new QStandardItemModel();
    ui->lex->setModel(data);
}

tj_lexical::~tj_lexical()
{
    delete ui;
}

void tj_lexical::modelinit()
{
    data->clear();
    data->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("Type")));
    data->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Value")));
}

void tj_lexical::receiveFile(QFile *a)
{
    ui->fileDisplayBrowser->clear();
    modelinit();

    selectedfile=a;
    QString filetext;
    if(!selectedfile->open(QIODevice::ReadOnly))
        exit(0);
    QTextStream *in=new QTextStream(selectedfile);
    while(!in->atEnd()){
        filetext=(*in).readLine();
        ui->fileDisplayBrowser->append(filetext);
    }
    delete in;
    selectedfile->close();
    if(!selectedfile->open(QIODevice::ReadOnly))
        exit(0);
    infile=new QTextStream(selectedfile);
}

void tj_lexical::receivelist(list<c_linklist> *a)
{
    m_Lexical=a;
}

void tj_lexical::on_oneStep_clicked()
{
    c_linklist node;
    do{
        c_scanner(*infile,*m_Lexical,node);
    }while(node.m_type==""||node.m_type==" ");
    int row=m_Lexical->size()-1;
    data->setItem(row,0,new QStandardItem(QString::fromStdString(node.m_type)));
    data->setItem(row,1,new QStandardItem(QString::fromStdString(node.m_value)));
}

int tj_lexical::c_scanner(QTextStream &infile, list<c_linklist> &m_Lexical, c_linklist &onode)
{
    //用于存储扫描到的词语
    c_linklist node;
    char ch;
    if (!infile.atEnd()){
        infile >> ch;
    }
    else
        return 2;
    //过滤空格
    while (ch == ' '||ch =='\n'||ch =='\t')
        if (!infile.atEnd()){
            infile >> ch;
        }
        else
            return 2;
    node.m_value = ch;
    if (ch >= 'a'&&ch <= 'z' || ch >= 'A'&&ch <= 'Z')
    {
        //标识符或者保留字
        if (!infile.atEnd()) {
            infile >> ch;
            while (ch >= 'a'&&ch <= 'z' || ch >= 'A'&&ch <= 'Z' || ch <= '9'&&ch >= '0') {
                node.m_value += ch;
                if (!infile.atEnd()){
                    infile >> ch;
                }
                else
                    break;
            }
            //将ch送回字符流中
            if (!infile.atEnd()){
                infile.seek(infile.pos()-1);
            }
        }
        set<TYPE>::iterator ite;
        if ((ite = conststr.find(node.m_value)) != conststr.end())
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
        if (!infile.atEnd()) {
            infile >> ch;
            node.m_value += ch;
            string::iterator ite;
            while (!(ch == '\"'&&*(ite = node.m_value.end() - 1) != '\\'))
            {
                ////考虑转义符
                if (!infile.atEnd()) {
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
        if (!infile.atEnd()) {
            infile >> ch;
            if (ch == '\\') {
                infile >> ch;
                node.m_value += ch;
                infile >> ch;
            }
            else {
                node.m_value += ch;
                if (!infile.atEnd()){
                    infile >> ch;
                }
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
        if (!infile.atEnd()) {
            infile >> ch;
            if (ch == '/')
                //单行注释，找\n
            {
                while (ch != '\n')
                    if (!infile.atEnd()){
                        infile >> ch;
                    }
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
        m_Lexical.push_back(node);
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
                if (!m_Lexical.empty() && isOperator(m_Lexical.back().m_type)) {
                    if (!infile.atEnd())
                    {
                        infile >> ch;
                        if (ch == '=')
                            node.m_value += ch;
                        else if (ch == '+')
                            node.m_value += ch;
                        else
                            //将字符送回字符流
                            infile.seek(infile.pos()-1);
                    }
                    node.m_type = node.m_value;
                }
                else {
                    findNumber(infile, node);
                }
                break;
            case '-':
                //-|-=
                if (!m_Lexical.empty() && isOperator(m_Lexical.back().m_type)) {
                    //是一个算符
                    if (!infile.atEnd()) {
                        infile >> ch;
                        if (ch == '=')
                            node.m_value += ch;
                        else if (ch == '-')
                            node.m_value += ch;
                        else
                            //将字符送回字符流
                            infile.seek(infile.pos()-1);
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
                if (!infile.atEnd()) {
                    infile >> ch;
                    if (ch == '=')
                        node.m_value += ch;
                    else
                        //将字符送回字符流
                        infile.seek(infile.pos()-1);
                }
                node.m_type = node.m_value;
                break;
            case '/':
                ///|/=
                if (!infile.atEnd()) {
                    infile >> ch;
                    if (ch == '=')
                        node.m_value += ch;
                    else
                        //将字符送回字符流
                        infile.seek(infile.pos()-1);
                }
                node.m_type = node.m_value;
                break;
            case ':':
                //:|::
                if (!infile.atEnd()) {
                    infile >> ch;
                    if (ch == ':')
                        node.m_value += ch;
                    else
                        //将字符送回字符流
                        infile.seek(infile.pos()-1);
                }
                node.m_type = node.m_value;
                break;
            case '&':
                //&|&&
                if (!infile.atEnd()) {
                    infile >> ch;
                    if (ch == '&')
                        node.m_value += ch;
                    else
                        //将字符送回字符流
                        infile.seek(infile.pos()-1);
                }
                node.m_type = node.m_value;
                break;
            case '|':
                //||||
                if (!infile.atEnd()) {
                    infile >> ch;
                    if (ch == '|')
                        node.m_value += ch;
                    else
                        //将字符送回字符流
                        infile.seek(infile.pos()-1);
                }
                node.m_type = node.m_value;
                break;
            case '=':
                //=|==
                if (!infile.atEnd()) {
                    infile >> ch;
                    if (ch == '=')
                        node.m_value += ch;
                    else
                        //将字符送回字符流
                        infile.seek(infile.pos()-1);
                }
                node.m_type = node.m_value;
                break;
            case '>':
                //>|>>|>=
                if (!infile.atEnd()) {
                    infile >> ch;
                    if (ch == '=')
                        node.m_value += ch;
                    else	if (ch == '>')
                        node.m_value += ch;
                    else
                        //将字符送回字符流
                        infile.seek(infile.pos()-1);
                }
                node.m_type = node.m_value;
                break;
            case '<':
                //<|<<|<=
                if (!m_Lexical.empty()&&m_Lexical.back().m_type != "INCLUDE") {
                    if (!infile.atEnd()) {
                        infile >> ch;
                        if (ch == '=')
                            node.m_value += ch;
                        else	if (ch == '<')
                            node.m_value += ch;
                        else
                            //将字符送回字符流
                            infile.seek(infile.pos()-1);
                    }
                    node.m_type = node.m_value;
                }
                else {
                    if (!infile.atEnd()) {
                        infile >> ch;
                        node.m_value += ch;
                        while (ch != '>')
                        {
                            if (!infile.atEnd()) {
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
                if (!infile.atEnd()) {
                    infile >> ch;
                    if (ch == '=')
                        node.m_value += ch;
                    else
                        //将字符送回字符流
                        infile.seek(infile.pos()-1);
                }
                node.m_type = node.m_value;
                break;
        }
    }
    onode.m_type=node.m_type;
    onode.m_value=node.m_value;
    m_Lexical.push_back(node);
    return 1;
}

void tj_lexical::on_toEnd_clicked()
{
    int ret;
    c_linklist onode;
    while ((ret=c_scanner(*infile, *m_Lexical,onode)) != 2&&ret!=-1)
        ;
    c_linklist last("#", "#");
    m_Lexical->push_back(last);
    list<c_linklist>::iterator ite = m_Lexical->begin();
    int i=0;
    while (ite != m_Lexical->end())
    {
        data->setItem(i,0,new QStandardItem(QString::fromStdString((*ite).m_type)));
        data->setItem(i,1,new QStandardItem(QString::fromStdString((*ite).m_value)));
        ite++;
        i++;
    }
}
