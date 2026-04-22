#include "mainwindow.h"
#include <QApplication>
#include <QFile>

#include "HardwareControlPage.h"
#include "HardwareMonitorPage.h"
#include "MusicPage.h"
#include "VideoPage.h"

int main(int argc, char *argv[]) {
  // QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QApplication a(argc, argv);

  QFile file(":/res/style/base.qss");
  if (file.open(QFile::ReadOnly)) {
    QString styleSheet = QLatin1String(file.readAll());
    a.setStyleSheet(styleSheet);
    file.close();
  }

  // 实例化新的主窗口系统
  MainWindow w;

  // 实例化各独立应用子模块，并注入解耦的微系统
  MusicPage *musicApp = new MusicPage(&w);
  VideoPage *videoApp = new VideoPage(&w);
  HardwareMonitorPage *monitorApp = new HardwareMonitorPage(&w);

  // 增量式注册应用模块，此时主窗口不再通过数字硬编码寻找应用
  w.registerApp(musicApp);
  w.registerApp(videoApp);
  w.registerApp(monitorApp);
  HardwareControlPage *hwCtrlApp = new HardwareControlPage(&w);
  w.registerApp(hwCtrlApp);

  w.showFullScreen();

  return a.exec();
}
