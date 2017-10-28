#ifndef TJ_LEXICAL_H
#define TJ_LEXICAL_H

#include <QStandardItemModel>
#include <QTextStream>
#include <QDialog>
#include <QFile>
#include <QString>
#include "main.h"
struct c_linklist
{
    TYPE m_type;
    VALUE m_value;
    c_linklist(string,string);
    c_linklist();
};
namespace Ui {
class tj_lexical;
}

class tj_lexical : public QDialog
{
    Q_OBJECT

public:
    explicit tj_lexical(QWidget *parent = 0);
    QStandardItemModel *data;
    ~tj_lexical();

private:
    Ui::tj_lexical *ui;
    QFile *selectedfile;
    list<c_linklist>* m_Lexical;
    QTextStream *infile;
    void modelinit();
    int c_scanner(QTextStream &infile,list<c_linklist> &m_Lexical,c_linklist &onode);//扫描器，每次扫描出一个保留字，标识符或数字
private slots:
    void receiveFile(QFile *a);
    void receivelist(list<c_linklist> *a);
    void on_oneStep_clicked();
    void on_toEnd_clicked();
};

STATUS findNumber(QTextStream &infile, c_linklist &node);

bool isOperator(const TYPE &a);

#endif // TJ_LEXICAL_H
