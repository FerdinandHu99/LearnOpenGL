#include "hfwidget.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HFWidget w;
    w.show();

    return a.exec();
}
