#include "unvisualkode.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UnvisualKode w;
    w.show();
    return a.exec();
}
