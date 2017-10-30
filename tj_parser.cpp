#include "tj_parser.h"
#include "ui_tj_parser.h"
#include <QStack>
#include <QVector>
#include <QMessageBox>

QStack<QChar>symbol;
QStack<QStandardItem *>symbolpoint;
//记录前一个项
QString ex;
int start=0;
tj_parser::tj_parser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tj_parser)
{
    ui->setupUi(this);
    ui->forecast->setEnabled(false);
    ui->oneStep->setEnabled(false);
    ui->multiStep->setEnabled(false);
}

tj_parser::~tj_parser()
{
    delete ui;
}

int tj_parser::getFOLLOW(const QChar &A, map<QChar, QString>&F, map<QChar, QString>&FIRST)
{
    for (int i = 0; i < N; i++)
    {
        int pos;
        QString a = Sentence->item(i)->text();
        if ((pos = a.indexOf(A, 2)) != -1)
        {
            if (!(a[pos + 1] == '|' || pos + 1 == a.length()))
            {
                //若有产生式B→αAβ,
                //则将FIRST(β) - {ε}加到FOLLOW(A)中;
                //若B →αAβ, 且β可以推导出ε
                //则将FOLLOW(B)加到FOLLOW(A)中；
                QString b = FIRST[a[pos + 1]];
                F[A] += b;
                int pos;
                if((pos = F[A].indexOf('@'))!=-1)
                    F[A].remove(pos,1);
                if (b.indexOf('@') != -1)
                {
                    QString c = getFollow(Sentence->item(i)->text()[0], F, FIRST);
                    F[A] += c;
                }

            }
            else
            {
                //B→αA
                F[A] += getFollow(a[0], F, FIRST);
            }
        }
    }
    return 1;
}

QString tj_parser::getFollow(const QChar &A, map<QChar, QString>&F, map<QChar, QString>&FIRST)//此函数和getF函数是用来产生一个递归。
{
    if (F[A] != "")
        return F[A];
    else
    {
        getFOLLOW(A, F, FIRST);
        return F[A];
    }
}

int tj_parser::getFirst(const QString &A, map<QChar, QString>&F)
{
    int ite;
    QChar ch = A[0];
    if (F[ch] != "")
        return 1;
    for (ite = 2; ite < A.length(); ite++)
    {
        QString k = getF(A.toStdString()[ite], F);
        if (k.indexOf('@') != -1)
        {
            int t = k.indexOf('@');
            if (A[ite] == '@')
                F[ch] += '@';
            else
                F[ch] += k.remove(t, 1);
        }
        else
        {
            F[ch] += k;
            int i = A.indexOf('|', ite);
            if (ite == i)F[ch] += '@';
            ite = i;
            if (ite == -1)//就已经找完了
                break;
        }
    }
    return 1;
}

QString tj_parser::getF(const QChar &A, map<QChar, QString>&F)
{
    QString str;
    if (A == '|')
        return "";
    if (F[A] == "")
    {
        for (int i = 0; i < N; i++)
            if (Sentence->item(i)->text()[0] == A)
                str = Sentence->item(i)->text();
        getFirst(str, F);
        return F[A];
    }
    else
    {
        return F[A];
    }
}

void tj_parser::receivelistfromtop(list<c_linklist> *a)
{
    Lexical = a;
    analyseite = Lexical->begin();
}

void tj_parser::receivemodelfromtop(modelpack *a)
{
    Sentence = a->Sentence;
    Lexicaldata = a->Lexicaldata;
    ui->parserData->setModel(Sentence);
    ui->lexicalData->setModel(Lexicaldata);
    N = Sentence->rowCount();
}

void tj_parser::receivedatafromtop(datapack *a)
{
    VT = a->VT;
    VN = a->VN;
}

void tj_parser::on_firstfollow_clicked()
{
    //产生每个符号的FIRST集合、FOLLOW集合
    //FIRST集合
    first = new map<QChar,QString>();
    set<QChar>::iterator ite;
    for (ite = VN->begin(); ite != VN->end(); ite++)
    {
        QChar ch = *ite;
        (*first)[ch] = ch;
    }
    for (int i = 0; i < N; i++)
    {
        getFirst(Sentence->item(i)->text(), *first);
    }
    for (ite = VT->begin(); ite != VT->end(); ite++)
    {
        (*first)[*ite] = m_erase((*first)[*ite]);
    }
    //FOLLOW集合
    follow = new map<QChar,QString>();
    (*follow)[Sentence->item(0)->text().toStdString()[0]] = '#';
    for (int i = 0; i < N; i++)
    {
        QChar k = Sentence->item(i)->text()[0];
        getFOLLOW(k, (*follow), (*first));
        (*follow)[k] = m_erase((*follow)[k]);
    }
    QStandardItemModel *firstmodel = new QStandardItemModel();
    firstmodel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("非终结字符")));
    firstmodel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("终结首符集")));
    map<QChar,QString>::iterator ite1;
    int i=0;
    for(ite1=first->begin();ite1!=first->end();i++,ite1++)
    {
        firstmodel->setItem(i,0,new QStandardItem((*ite1).first));
        firstmodel->setItem(i,1,new QStandardItem((*ite1).second));
    }
    QStandardItemModel *followmodel = new QStandardItemModel();
    followmodel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("非终结字符")));
    followmodel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("终结尾符集")));
    i=0;
    for(ite1=follow->begin();ite1!=follow->end();i++,ite1++)
    {
        followmodel->setItem(i,0,new QStandardItem((*ite1).first));
        followmodel->setItem(i,1,new QStandardItem((*ite1).second));
    }
    ui->firsttableView->setModel(firstmodel);
    ui->followtableView->setModel(followmodel);

    ui->forecast->setEnabled(true);
}

bool tj_parser::isRELOP(QString k)
{
    if (k == "<" || k == "<=" || k == ">" || k == ">=" || k == "==" || k == "!="||k=="&"||k=="&&"||k=="||"||k=="|")
        return 1;
    return 0;
}

QString &tj_parser::m_erase(QString &a)
{
    for (int i = 0; i < a.length(); i++)
    {
        for (int j = i + 1; j < a.length(); j++)
        {
            if (a[j] == a[i])
            {
                a.remove(j, 1);
                j--;
            }
        }
    }
    return a;
}

int tj_parser::findtype(QString k)
{
    QString m_kname[8] = {
        "INT","VOID","IDENTIFIER","RETURN","WHILE","ELSE","NUM","IF"
    };
    for (int i = 0; i < 8; i++)
    {
        if (m_kname[i] == k)
            return i;
    }
    if(k.contains("CONST",Qt::CaseSensitive))
        return 6;
    if(k=="INT"||k=="DOUBLE"||k=="CHAR")
        return 0;
    return -1;
}

void tj_parser::on_forecast_clicked()
{
    Forecast = new map<QString,QString>;
    stringstream sstr;
    for (int i = 0; i < N; i++)
    {
        QString sentences = Sentence->item(i)->text();
        int firpos = 2,secpos;
        secpos = sentences.indexOf('|');
        for (int i=firpos; i < sentences.length(); i++)
        {
            QChar ch = sentences[i];//对于每个属于FIRST集的终结符
            for (int j = 0; j < (*first)[ch].length(); j++)//第二部
            {
                QString key;
                string a;
                if ((*first)[ch][j] == '@')
                    continue;
                sstr<< sentences[0].toLatin1() << (*first)[ch][j].toLatin1();//构造键值Aa
                sstr >> a;
                key=QString::fromStdString(a);
                sstr.clear();
                int len = (secpos == -1) ? sentences.length() - firpos : secpos - firpos + 1;
                (*Forecast)[key] += sentences.mid(firpos, len);//将产生式加入到M(A,a)
            }
            if ((*first)[ch].indexOf('@') != -1)//此产生式的部分的首符集还没处理完
            {
                if (i + 1 == sentences.length() || sentences[i + 1] == '|')//说明FIRST[alpha]包含空，即是第三步
                {
                    for (int j = 0; j < (*follow)[sentences[0]].length(); j++)
                    {
                        QString key;
                        string a;
                        sstr << sentences[0].toLatin1() << (*follow)[sentences[0]][j].toLatin1();//构造键值Ab
                        sstr >> a;
                        key=QString::fromStdString(a);
                        sstr.clear();
                        int len = (secpos == -1) ? sentences.length() - firpos : secpos - firpos + 1;
                        (*Forecast)[key] += sentences.mid(firpos, len);//将产生式加入到M(A,b)
                    }
                }
                continue;
            }
            else//处理下一个产生式
            {
                if (secpos == -1)//第一行已经处理完了
                {
                    break;
                }
                firpos = secpos + 1;
                i = secpos;//接下来i还要++，所以不赋值firpos
                secpos = sentences.indexOf('|', secpos + 1);
            }
        }
    }
    map<QString,QString>::iterator ite2;
    QStandardItemModel *forecastmodel = new QStandardItemModel();
    forecastmodel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("左端符号")));
    forecastmodel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("产生式")));
    int row=0;
    for(ite2 = Forecast->begin();ite2 != Forecast->end();ite2++,row++)
    {
        forecastmodel->setItem(row,0,new QStandardItem((*ite2).first));
        forecastmodel->setItem(row,1,new QStandardItem((*ite2).second));
    }
    ui->forecasttableView->setModel(forecastmodel);

    ui->oneStep->setEnabled(true);
    ui->multiStep->setEnabled(true);
}

void tj_parser::on_multiStep_clicked()
{
    if(start==0){
        QStandardItemModel *model=new QStandardItemModel(ui->treeView);
        ui->treeView->setModel(model);

        QStandardItem *parentItem = model->invisibleRootItem();

        QStandardItem *item=new QStandardItem(QString("E"));
        parentItem->appendRow(item);
        c_linklist last(QString("#"),QString("#"));
        Lexical->push_back(last);
        symbol.push('#');
        symbol.push('E');
        symbolpoint.push(new QStandardItem(QString("#")));
        symbolpoint.push(item);
        start=1;
    }
    for (; analyseite != Lexical->end();)
    {
        stringstream sstr;
        QString posxy;//构建坐标
        QString k = (*analyseite).m_type;
        int pos = findtype(k);//找到该类型的编号
        QChar top = symbol.pop();//取出栈顶的符号
        QStandardItem *item = symbolpoint.pop();
        if (pos == -1)//是个符号
        {
            if (k[0] == top)
            {
                //是一个终结符而且匹配成功
                ex=analyseite->m_value;
                analyseite++;
                continue;
            }
            if ('g' == top && isRELOP(k))
            {
                //是一个终结符而且匹配成功
                ex=analyseite->m_value;
                analyseite++;
                continue;
            }
            if(isRELOP(k))
                sstr << top.toLatin1() << 'g';
            else
                sstr << top.toLatin1() << k[0].toLatin1();
            string a;
            sstr >> a;
            posxy=QString::fromStdString(a);
        }
        else//不是个符号，用编号进行匹配
        {
            sstr << top.toLatin1() << pos;
            string a;
            sstr >> a;
            posxy=QString::fromStdString(a);
        }
        if (pos == top.toLatin1() - '0')
        {
            //是一个终结符而且匹配成功
            QStandardItem *itemchild = new QStandardItem((*analyseite).m_value);
            item->appendRow(itemchild);
            ex=analyseite->m_value;
            analyseite++;
            continue;
        }
        QString pattern;
        if (Forecast->count(posxy) == 0)//出现了预测分析表之外的情况，报错
        {
                QMessageBox::warning(this,"Error!",QString("%1在遇到%2的时候没有产生式\n%3后不可跟%4")
                                     .arg(QChar(posxy[0]),QChar(posxy[1]),ex,analyseite->m_value));
                symbol.push(top);//取出栈顶的符号
                symbolpoint.push(item);
                break;
        }
        else
        {
            pattern = (*Forecast)[posxy];//查到产生式
            if (pattern == "@" || pattern == "@|") {
                QStandardItem *itemchild = new QStandardItem(QString("@"));
                item->appendRow(itemchild);
                continue;
            }
            QVector<QStandardItem *>itemchild;
            for (int i = pattern.length() - 1; i >= 0; i--)
            {
                if(pattern[i]!='|'){
                    symbol.push(pattern[i]);//逆序将产生式压栈
                    QStandardItem *itemchild1 = new QStandardItem(QChar(pattern[i]));
                    itemchild.append(itemchild1);
                    symbolpoint.push(itemchild1);
                }
            }
            QVector<QStandardItem *>::iterator ite123;
            for (ite123 = itemchild.end()-1; ite123 >= itemchild.begin(); ite123--)
            {
                 item->appendRow(*ite123);
            }
        }
    }
    QMessageBox::warning(this,"Finished!",QString("成功分析完毕"));
    ui->treeView->expandAll();
}

void tj_parser::on_oneStep_clicked()
{
    if(start==0)
    {
        QStandardItemModel *model=new QStandardItemModel(ui->treeView);
        ui->treeView->setModel(model);

        QStandardItem *parentItem = model->invisibleRootItem();

        QStandardItem *item=new QStandardItem(QString("E"));
        parentItem->appendRow(item);
        c_linklist last(QString("#"),QString("#"));
        Lexical->push_back(last);
        symbol.push('#');
        symbol.push('E');
        symbolpoint.push(new QStandardItem(QString("#")));
        symbolpoint.push(item);
        start=1;
    }
    if(analyseite==Lexical->end())
    {
        QMessageBox::warning(this,"Finished!","成功分析完毕");
    }
    else
    {
        stringstream sstr;
        QString posxy;//构建坐标
        QString k = (*analyseite).m_type;
        int pos = findtype(k);//找到该类型的编号
        QChar top = symbol.pop();//取出栈顶的符号
        QStandardItem *item = symbolpoint.pop();
        if (pos == -1&&(k[0] == top||('g' == top && isRELOP(k))))//是一个终结符而且匹配成功
        {
            if (k[0] == top)
            {
                //是一个终结符而且匹配成功
                ex=analyseite->m_value;
                analyseite++;
            }
            else if ('g' == top && isRELOP(k))
            {
                //是一个终结符而且匹配成功
                ex=analyseite->m_value;
                analyseite++;
            }
        }
        else//不是个符号，用编号进行匹配
        {
            if(pos==-1)
                   if(isRELOP(k))
                       sstr << top.toLatin1() << 'g';
                   else
                   {
                       sstr << top.toLatin1() << k[0].toLatin1();
                       string a;
                       sstr >> a;
                       posxy=QString::fromStdString(a);
                   }
            else{
                sstr << top.toLatin1() << pos;
                string a;
                sstr >> a;
                posxy=QString::fromStdString(a);
            }
            if (pos == top.toLatin1() - '0')
            {
                    //是一个终结符而且匹配成功
                    QStandardItem *itemchild = new QStandardItem((*analyseite).m_value);
                    item->appendRow(itemchild);
                    ex=analyseite->m_value;
                    analyseite++;
            }
            else
            {
                    QString pattern;
                    if (Forecast->count(posxy) == 0)//出现了预测分析表之外的情况，报错
                    {

                        QMessageBox::warning(this,"Error!",QString("%1在遇到%2的时候没有产生式\n%3后不可跟%4")
                                             .arg(QChar(posxy[0]),QChar(posxy[1]),ex,analyseite->m_value));
                        symbol.push(top);//取出栈顶的符号
                        symbolpoint.push(item);
                    }
                    else
                    {
                        pattern = (*Forecast)[posxy];//查到产生式
                        if (pattern == "@" || pattern == "@|")
                        {
                                QStandardItem *itemchild = new QStandardItem(QString("@"));
                                item->appendRow(itemchild);
                        }
                        else
                        {
                            QVector<QStandardItem *>itemchild;
                            for (int i = pattern.length() - 1; i >= 0; i--)
                            {
                                if(pattern[i]!='|'){
                                    symbol.push(pattern[i]);//逆序将产生式压栈
                                    QStandardItem *itemchild1 = new QStandardItem(QChar(pattern[i]));
                                    itemchild.append(itemchild1);
                                    symbolpoint.push(itemchild1);
                                }
                            }
                            QVector<QStandardItem *>::iterator ite123;
                            for (ite123 = itemchild.end()-1; ite123 >= itemchild.begin(); ite123--)
                            {
                                 item->appendRow(*ite123);
                            }
                        }
                    }
             }
        }
    }
    ui->treeView->expandAll();
}
