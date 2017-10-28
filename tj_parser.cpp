#include "tj_parser.h"
#include "ui_tj_parser.h"

tj_parser::tj_parser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tj_parser)
{
    ui->setupUi(this);
}

tj_parser::~tj_parser()
{
    delete ui;
}

void tj_parser::receivelistfromtop(list<c_linklist> *a)
{
    Lexical = a;
}

void tj_parser::receivemodelfromtop(modelpack *a)
{
    Sentence = a->Sentence;
    Lexicaldata = a->Lexicaldata;
    ui->parserData->setModel(Sentence);
    ui->lexicalData->setModel(Lexicaldata);
}

void tj_parser::receivedatafromtop(datapack *a)
{
    VT = a->VT;
    VN = a->VN;
}

