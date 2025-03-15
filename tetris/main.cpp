#include "mainwindow.h"

#include <QApplication>

MainWindow* MainWnd{nullptr};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    MainWnd = &w;
    w.show();
    return a.exec();
}
