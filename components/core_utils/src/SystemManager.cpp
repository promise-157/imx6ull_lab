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
  if (m_isRunning) {
    qWarning() << "[SystemManager] Cannot register module"
               << module->moduleName() << "after startAll()!";
    return;
  }

  // 转移所有权到管家，由管家同一释放
  m_modules.append(module);

  if (requireNewThread) {
    QThread *thread = new QThread(this);
    // 让线程名字清晰，便于Top排查
    thread->setObjectName(module->moduleName() + "_Thread");

    // 关键1：绑定线程启动的信号到模块的 onStart 槽（这会在新线程里执行）
    connect(thread, &QThread::started, module, &ILifecycleModule::onStart);

    // 关键2：把对象推到新的线程空间
    module->moveToThread(thread);
    m_threads.append(thread);
  } else {
    // 如果不需要新线程，直接利用当前所在线程(一般是主线程或共享后台线程)
    // 这个可以未来扩展
  }
}

void SystemManager::startAll() {
  if (m_isRunning)
    return;

  // 第一步：在主线程，集体统一完成各个组件的资源静态探测或配置读取
  for (auto mod : m_modules) {
    qInfo() << "[SystemManager] Initializing:" << mod->moduleName();
    mod->onInit();
  }

  // 第二步：集体点火。所有通过moveToThread到子线程的模块，其onStart会随着线程的start并发拉起
  for (auto th : m_threads) {
    th->start();
  }

  m_isRunning = true;
  qInfo() << "[SystemManager] All registered background modules are up and "
             "running!";
}

void SystemManager::stopAll() {
  if (!m_isRunning)
    return;

  for (auto mod : m_modules) {
    qInfo() << "[SystemManager] Stopping:" << mod->moduleName();
    // 这里必须用跨线程调用的方式告诉他们在自己的线程里清理资源
    QMetaObject::invokeMethod(mod, "onStop", Qt::BlockingQueuedConnection);
  }

  for (auto th : m_threads) {
    th->quit();
    th->wait(2000); // 最多等两秒让其自行结束，防止死锁卡死退出
  }

  // 清理创建的线程（挂在父节点上的自己会回收，也可以明确清理）
  qDeleteAll(m_threads);
  m_threads.clear();

  // 如果模块父节点不是本实例，或者防止泄露
  // 由于通常在管家退出时整个app结束，可以由Qt对象树自动释放，这里手动更保险
  qDeleteAll(m_modules);
  m_modules.clear();

  m_isRunning = false;
  qInfo() << "[SystemManager] All background modules shut down successfully.";
}