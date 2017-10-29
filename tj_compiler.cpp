#include "tj_compiler.h"
#include "ui_tj_compiler.h"
tj_compiler::tj_compiler(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tj_compiler)
{
    ui->setupUi(this);
    ui->parserfileInput=ui->fileInput=NULL;

    ui->fileDisplayBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->allBtn->setEnabled(false);
    ui->lexicalBtn->setEnabled(false);
    ui->parserBtn->setEnabled(false);
    m_Lexical=new list<c_linklist>();
}

tj_compiler::~tj_compiler()
{
    delete ui;
}

void tj_compiler::on_fileInput_clicked()
{
    ui->fileDisplayBrowser->clear();

    QString filedir,filetext;
    QFileDialog *filedialog=new QFileDialog(this);

    filedialog->setViewMode(QFileDialog::List);
    filedialog->exec();
    filedir=(filedialog->selectedFiles())[0];

    selectedfile=new QFile(filedir);
    if(!selectedfile)
        exit(0);
    if(!selectedfile->open(QIODevice::ReadOnly))
        exit(0);
    QTextStream *in=new QTextStream(selectedfile);

    while(!in->atEnd()){
        filetext=(*in).readLine();
        ui->fileDisplayBrowser->append(filetext);
    }

    selectedfile->close();
    ui->lexicalBtn->setEnabled(true);
    if(parserselectedfile)
        ui->allBtn->setEnabled(true);

    delete m_Lexical;
    m_Lexical=new list<c_linklist>();
}

void tj_compiler::on_parserBtn_clicked()
{
    p=new tj_parser();
    connect(this, SIGNAL(sendmodeltoparser(modelpack *)), p, SLOT(receivemodelfromtop(modelpack *)));
    connect(this, SIGNAL(sendlisttoparser(list<c_linklist>*)), p, SLOT(receivelistfromtop(list<c_linklist> *)));
    connect(this, SIGNAL(senddatatoparser(datapack*)), p, SLOT(receivedatafromtop(datapack*)));

    emit senddatatoparser(new datapack(VT,VN));
    emit sendlisttoparser(m_Lexical);
    emit sendmodeltoparser(new modelpack(SentencePatternData,lexicaldata));
    p->exec();

}

void tj_compiler::on_lexicalBtn_clicked()
{
    w=new tj_lexical();
    connect(this, SIGNAL(sendFile(QFile *)), w, SLOT(receiveFile(QFile *)));
    connect(this, SIGNAL(sendlist(list<c_linklist> *)), w, SLOT(receivelist(list<c_linklist> *)));

    emit sendFile(selectedfile);
    emit sendlist(m_Lexical);
    w->exec();
    lexicaldata = w->data;//获取词法分析器的结果

    ui->parserBtn->setEnabled(true);
}

void tj_compiler::on_parserfileInput_clicked()
{
    QString filedir,filetext;
    QFileDialog *filedialog=new QFileDialog(this);

    filedialog->setViewMode(QFileDialog::List);
    filedialog->exec();
    filedir=(filedialog->selectedFiles())[0];

    parserselectedfile=new QFile(filedir);
    if(!parserselectedfile)
        exit(0);
    if(!parserselectedfile->open(QIODevice::ReadOnly))
        exit(0);
    QTextStream *in=new QTextStream(parserselectedfile);

    handleFile(*in);//读入语法文件并做简单处理
    ui->listView->setModel(SentencePatternData);

    parserselectedfile->close();//关闭文件

    if(selectedfile)
        ui->allBtn->setEnabled(true);

}

void tj_compiler::handleFile(QTextStream &infile)
{
    int N;
    infile>>N;

    VT = new set<QChar>();
    VN = new set<QChar>();
    SentencePattern = new QString[N];
    SentencePatternData = new QStandardItemModel();
    for (int i = 0; i < N; i++)
    {
        infile >> SentencePattern[i];
        SentencePatternData->setItem(i,new QStandardItem(SentencePattern[i]));
        VT->insert(SentencePattern[i][0]);
        for (int j = 2; j < SentencePattern[i].length(); j++)
        {
            VN->insert(SentencePattern[i][j]);
        }
    }
    VN->erase('|');
    set<QChar>::iterator ite;
    for (ite = VT->begin(); ite != VT->end(); ite++)
        VN->erase(*ite);
}
