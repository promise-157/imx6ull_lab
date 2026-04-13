#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.showFullScreen(); // i.MX6ULL 全屏运行
    return a.exec();
}