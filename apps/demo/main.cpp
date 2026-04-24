#include "mainwindow.h"
#include <QApplication>
#include <QFile>

#include "HardwareControlPage.h"
#include "HardwareMonitorPage.h"
#include "MusicPage.h"
#include "VideoPage.h"

// 引入系统架构支撑
#include "LoggerManager.h"
#include "SystemManager.h"
#include "ConfigManager.h"

// 引入应用级装配清单，实现与具体底层类的绝对隔离
#include "AppBootstrap.h"

int main(int argc, char *argv[]) {
  // QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QApplication a(argc, argv);

  // --- 1. 激活工业级日志监控 ---
  LoggerManager::instance().init("./logs");
  qInfo() << "[Main] Application starting...";

  // --- 1.5 加载全局默认配置（JSON） ---
  ConfigManager::instance().loadConfig(QCoreApplication::applicationDirPath() +
                                       "/config.json");

  // --- 2. 聚合注册所有子系统（黑盒装配） ---
  AppBootstrap::initAllSubsystems();

  // 系统大管家一键点火：分配线程、触发初始化
  SystemManager::instance().startAll();

  // --- 3. 启动 UI 基座 ---
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
  HardwareControlPage *hwCtrlApp = new HardwareControlPage(&w);

  // 增量式注册应用模块，此时主窗口不再通过数字硬编码寻找应用
  w.registerApp(musicApp);
  w.registerApp(videoApp);
  w.registerApp(monitorApp);
  w.registerApp(hwCtrlApp);

  w.showFullScreen();

  int ret = a.exec();

  // 安全停止所有后台服务
  SystemManager::instance().stopAll();

  return ret;
}
