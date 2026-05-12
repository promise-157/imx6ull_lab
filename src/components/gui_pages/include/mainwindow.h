#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "IAppModule.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QList>
#include <QMainWindow>
#include <QMap>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow(QWidget *parent = nullptr);

  // 页面注册接口：将任意继承自 IAppModule 的页面注册到系统
  void registerApp(IAppModule *app);

private slots:
  void goHome() { m_stack->setCurrentIndex(0); }

  // 当 App 请求收起后台时调用
  void onAppMinimize(IAppModule *app);
  // 当 App 请求彻底退出时调用
  void onAppClose(IAppModule *app);

private:
  QStackedWidget *m_stack;

  // 系统顶部任务栏相关
  QHBoxLayout *m_taskBarLayout;
  QWidget *m_systemTopBar;

  // 已注册的所有应用列表
  QList<IAppModule *> m_registeredApps;
  // 后台运行的应用与对应图标按钮的映射
  QMap<IAppModule *, QPushButton *> m_runningApps;
  // 桌面应用的快捷方式容器
  QGridLayout *m_desktopGrid;

  void setupSystemTopBar(QVBoxLayout *mainLayout);
  QWidget *createDesktopPage();

  // 更新后台栏的显示状态
  void addAppToTaskBar(IAppModule *app);
  void removeAppFromTaskBar(IAppModule *app);
};

#endif
