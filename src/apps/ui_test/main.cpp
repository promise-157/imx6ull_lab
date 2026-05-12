#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[]) {
  // QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QApplication a(argc, argv);

  // --- 3. 启动 UI 基座 ---
  QFile file(":/res/style/base.qss");
  if (file.open(QFile::ReadOnly)) {
    QString styleSheet = QLatin1String(file.readAll());
    a.setStyleSheet(styleSheet);
    file.close();
  }

  // 实例化新的主窗口系统
  MainWindow w;

  w.showFullScreen();

  int ret = a.exec();

  return ret;
}
