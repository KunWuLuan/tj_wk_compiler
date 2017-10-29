#ifndef TJ_PARSER_H
#define TJ_PARSER_H
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
    set<QChar> *VT;
    set<QChar> *VN;
    datapack(set<QChar> *a,set<QChar> *b)
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

    int getFOLLOW(const QChar &A, map<QChar, QString>&F, map<QChar, QString>&FIRST);
    QString getFollow(const QChar &A, map<QChar, QString>&F, map<QChar, QString>&FIRST);
    int getFirst(const QString &A, map<QChar, QString>&F);
    QString getF(const QChar &A, map<QChar, QString>&F);
    bool isRELOP(QString k);
    QString & m_erase(QString &a);
private:
    Ui::tj_parser *ui;

    int Parser(list<c_linklist> Lexical);

    int N;
    list<c_linklist> *Lexical;
    QStandardItemModel *Sentence;
    QStandardItemModel *Lexicaldata;
    set<QChar> *VT;//终结字符集
    set<QChar> *VN;//非终结字符集
    map<QChar,QString> *first;
    map<QChar,QString> *follow;
    map<QString, QString> *Forecast;
    list<c_linklist>::iterator analyseite;//记录分析进度
    int findtype(QString k);

private slots:
    void receivelistfromtop(list<c_linklist> *a);
    void receivemodelfromtop(modelpack *a);
    void receivedatafromtop(datapack *a);
    void on_firstfollow_clicked();
    void on_forecast_clicked();
    void on_multiStep_clicked();
    void on_oneStep_clicked();
};

#endif // TJ_PARSER_H
