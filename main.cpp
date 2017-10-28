#include "tj_compiler.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    tj_compiler w;
    w.show();

    return a.exec();
}
