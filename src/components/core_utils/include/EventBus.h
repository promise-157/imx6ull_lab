#pragma once
#include <QObject>
#include <QString>
#include <QVariant>

class EventBus : public QObject {
  Q_OBJECT
public:
  // 获取单例全局总线
  static EventBus *getInstance() {
    static EventBus instance;
    return &instance;
  }

  // [发布者调用] 可以在任何线程调用，发布事件与负载数据
  static void publish(const QString &topic,
                      const QVariant &payload = QVariant()) {
    emit getInstance()->eventOccurred(topic, payload);
  }

  // [订阅者调用] 订阅指定的 topic。
  // 特点1：传入 receiver 对象 (通常为 this)，当 receiver
  // 被析构时，这里的自动解绑，防止野指针闪退。 特点2：保证回调函数 callback
  // 永远运行在 receiver 所在的线程，实现天然的跨线程安全通信。
  // 特点3：支持简单的 MQTT "#" 通配符。如 "hw/req/#"
  template <typename Func>
  static void subscribe(const QString &topic, QObject *receiver,
                        Func callback) {
    connect(getInstance(), &EventBus::eventOccurred, receiver,
            [topic, callback](const QString &t, const QVariant &payload) {
              // 如果精确匹配，或者带有通配符 /# 前缀匹配
              if (t == topic ||
                  (topic.endsWith("/#") &&
                   t.startsWith(topic.left(topic.length() - 2)))) {
                callback(payload);
              }
            });
  }

signals:
  // Qt 内部底层派发的实际信号，依靠 Qt 的事件循环队列进行安全的线程派发
  void eventOccurred(const QString &topic, const QVariant &payload);

private:
  explicit EventBus(QObject *parent = nullptr) : QObject(parent) {}
};
