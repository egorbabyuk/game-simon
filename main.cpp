#include "mainwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWidget window;

    window.setFixedSize(600,450);
    window.show();

    return a.exec();
}
