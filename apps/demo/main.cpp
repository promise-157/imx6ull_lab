#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QThread>

#include "HardwareControlPage.h"
#include "HardwareMonitorPage.h"
#include "MusicPage.h"
#include "VideoPage.h"

// 引入解耦后的Service和HAL
#include "AudioHal.h"
#include "MediaService.h"

int main(int argc, char *argv[]) {
  // QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QApplication a(argc, argv);

  QFile file(":/res/style/base.qss");
  if (file.open(QFile::ReadOnly)) {
    QString styleSheet = QLatin1String(file.readAll());
    a.setStyleSheet(styleSheet);
    file.close();
  }

  // --- 1. 启动全局服务与HAL节点 ---
  // 它们自己内部封装了QThread，或者我们可以把HAL丢到单独线程去
  MediaService *mediaSvc =
      new MediaService(); // MediaService internally spans a worker thread

  // AudioHal也挂载到单独后台线程
  QThread *audioThread = new QThread();
  AudioHal *audioHal = new AudioHal();
  audioHal->moveToThread(audioThread);
  // 这里没有显式连接 quit和deleteLater，但因为随着app存在，不退出可以不处理
  audioThread->start();

  // --- 2. 启动UI层 ---

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
