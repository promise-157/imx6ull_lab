#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include "ILifecycleModule.h"
#include <QList>
#include <QObject>
#include <QThread>

// 系统大管家：管理所有的后台服务与硬件组件，负责线程分配与生命周期管理
class SystemManager : public QObject {
  Q_OBJECT
public:
  static SystemManager &instance();

  // 注册组件：管家内部会自动处理挂载线程并分配生命周期
  void registerModule(ILifecycleModule *module, bool requireNewThread = true);

  // 一键总启动
  void startAll();

  // 一键总关闭（可用于程序退出时安全释放所有后台资源）
  void stopAll();

private:
  SystemManager() = default;
  ~SystemManager();

  QList<ILifecycleModule *> m_modules;
  QList<QThread *> m_threads;
  bool m_isRunning = false;
};

#endif // SYSTEMMANAGER_H