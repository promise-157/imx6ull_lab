#include "SystemManager.h"
#include <QCoreApplication>
#include <QDebug>

SystemManager &SystemManager::instance() {
  static SystemManager _instance;
  return _instance;
}

SystemManager::~SystemManager() { stopAll(); }

void SystemManager::registerModule(ILifecycleModule *module,
                                   bool requireNewThread) {
  if (!module) {
    qWarning() << "[SystemManager] registerModule got nullptr.";
    return;
  }

  if (m_isRunning) {
    qWarning() << "[SystemManager] Cannot register module"
               << module->moduleName() << "after startAll()!";
    return;
  }

  ManagedModule managed;
  managed.module = module;

  if (requireNewThread) {
    QThread *thread = new QThread(this);
    thread->setObjectName(module->moduleName() + "_Thread");
    managed.thread = thread;
  }

  // 转移所有权到管家，由管家统一释放
  m_managedModules.append(managed);
}

void SystemManager::startAll() {
  if (m_isRunning)
    return;

  // 第一步：统一在主线程完成 onInit，避免 moveToThread 后跨线程直接调用
  for (const auto &entry : m_managedModules) {
    qInfo() << "[SystemManager] Initializing:" << entry.module->moduleName();
    entry.module->onInit();
  }

  // 第二步：根据模块配置点火
  for (auto &entry : m_managedModules) {
    if (entry.thread) {
      // 注意：moveToThread 必须在对象无父对象时进行。当前模块均以 nullptr 构造。
      entry.module->moveToThread(entry.thread);
      connect(entry.thread, &QThread::started, entry.module,
              &ILifecycleModule::onStart, Qt::UniqueConnection);
      entry.thread->start();
    } else {
      // 无独立线程的模块直接在当前线程启动（通常是主线程）
      entry.module->onStart();
    }
  }

  m_isRunning = true;
  qInfo() << "[SystemManager] All registered background modules are up and "
             "running!";
}

void SystemManager::stopAll() {
  if (m_managedModules.isEmpty())
    return;

  for (auto &entry : m_managedModules) {
    qInfo() << "[SystemManager] Stopping:" << entry.module->moduleName();

    if (entry.thread) {
      // 在模块所属线程中执行 onStop
      QMetaObject::invokeMethod(entry.module, "onStop",
                                Qt::BlockingQueuedConnection);
    } else {
      // 无独立线程模块直接同步停止
      entry.module->onStop();
    }
  }

  for (auto &entry : m_managedModules) {
    if (!entry.thread)
      continue;
    entry.thread->quit();
    entry.thread->wait(2000); // 最多等两秒让其自行结束，防止死锁卡死退出
  }

  for (auto &entry : m_managedModules) {
    delete entry.thread;
    entry.thread = nullptr;
  }

  for (auto &entry : m_managedModules) {
    delete entry.module;
    entry.module = nullptr;
  }
  m_managedModules.clear();

  m_isRunning = false;
  qInfo() << "[SystemManager] All background modules shut down successfully.";
}
