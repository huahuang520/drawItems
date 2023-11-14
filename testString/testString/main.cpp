#include "testString.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    testString w;
    w.show();
    return a.exec();
}
