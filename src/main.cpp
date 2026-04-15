#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    // 针对嵌入式屏幕的属性设置（可选）
    // QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);

    // 1. 加载全局基础样式表 (如通用的字体、全局背景、滚动条美化)
    QFile file(":/res/style/base.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        a.setStyleSheet(styleSheet);
        file.close();
    }

    // 2. 启动主窗口
    // 你的 MainWindow 构造函数会自动初始化 MusicPage 和 VideoPage
    MainWindow w;
    
    // 如果是嵌入式板子，通常直接全屏显示
    // w.showFullScreen(); 
    w.show();

    return a.exec();
}