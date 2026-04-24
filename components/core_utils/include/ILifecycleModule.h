#ifndef ILIFECYCLEMODULE_H
#define ILIFECYCLEMODULE_H

#include <QObject>
#include <QString>

// 生命周期组件接口：所有需要后台运行或者独立线程的组件，都必须继承它
class ILifecycleModule : public QObject {
  Q_OBJECT
public:
  explicit ILifecycleModule(QObject *parent = nullptr) : QObject(parent) {}
  virtual ~ILifecycleModule() = default;

  // 模块名称（用于日志和管家注册标识）
  virtual QString moduleName() const = 0;

  // 1. 初始化阶段 (在主线程执行，用于加载配置、前期探测等)
  virtual void onInit() = 0;

  // 2. 启动阶段 (在子线程执行，用于分配资源、监听 EventBus、启动定时器等)
  virtual void onStart() = 0;

  // 3. 停止阶段 (用于安全关闭文件、断开底层硬件等)
  virtual void onStop() = 0;
};

#endif // ILIFECYCLEMODULE_H