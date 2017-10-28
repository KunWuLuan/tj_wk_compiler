#ifndef TJ_PARSER_H
#define TJ_PARSER_H
#include "main.h"
#include "tj_lexical.h"
#include <QDialog>

namespace Ui {
class tj_parser;
}
struct modelpack
{
    QStandardItemModel *Sentence;
    QStandardItemModel *Lexicaldata;
    modelpack(QStandardItemModel *a,QStandardItemModel *b)
    {
        Sentence = a;
        Lexicaldata = b;
    }
};
struct datapack
{
    set<char> *VT;
    set<char> *VN;
    datapack(set<char> *a,set<char> *b)
    {
        VT = a;
        VN = b;
    }
};
class tj_parser : public QDialog
{
    Q_OBJECT

public:
    explicit tj_parser(QWidget *parent = 0);
    ~tj_parser();

private:
    Ui::tj_parser *ui;

    int Parser(list<c_linklist> Lexical);

    list<c_linklist> *Lexical;
    QStandardItemModel *Sentence;
    QStandardItemModel *Lexicaldata;
    set<char> *VT;//终结字符集
    set<char> *VN;//非终结字符集
    map<char,string> *first;
    map<char,string> *follow;

private slots:
    void receivelistfromtop(list<c_linklist> *a);
    void receivemodelfromtop(modelpack *a);
    void receivedatafromtop(datapack *a);
};

#endif // TJ_PARSER_H
