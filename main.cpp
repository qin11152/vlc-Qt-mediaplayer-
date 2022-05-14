#include "MyMediaplayer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyMediaplayer w;
    w.show();
    return a.exec();
}
