#ifndef TJ_COMPILER_H
#define TJ_COMPILER_H
#include<QFileDialog>
#include<QMainWindow>
#include "tj_parser.h"
#include "main.h"
namespace Ui {
class tj_compiler;
}

class tj_compiler : public QMainWindow
{
    Q_OBJECT

public:
    explicit tj_compiler(QWidget *parent = 0);
    ~tj_compiler();

private slots:
    void on_fileInput_clicked();

    void on_parserBtn_clicked();

    void on_lexicalBtn_clicked();

    void on_parserfileInput_clicked();

private:
    Ui::tj_compiler *ui;
    tj_lexical *w;
    tj_parser *p;
    QFile *selectedfile;//选取的要编译的文件
    QFile *parserselectedfile;//选取的语法规则的文件
    QString *SentencePattern;
    QStandardItemModel *lexicaldata;//存储分析出的词法数据
    QStandardItemModel *SentencePatternData;//存储分析出的词法数据
    list<c_linklist> *m_Lexical;//定义的词法分析器结果链表
    set<char> *VT;//终结字符集
    set<char> *VN;//非终结字符集
    map<char,string> *first;
    map<char,string> *follow;

    void handleFile(QTextStream &infile);
signals:
    void sendFile(QFile *);
    void sendlist(list<c_linklist>* a);
    void sendmodeltoparser(modelpack *);
    void sendlisttoparser(list<c_linklist> *a);
    void senddatatoparser(datapack *a);
};

#endif // TJ_COMPILER_H
