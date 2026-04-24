#include "HardwareHal.h"
#include <QDebug>
#include <QFile>
#include <QRandomGenerator>
#include <QTextStream>
#include <QThread>
#include <QVariant>

HardwareHal::HardwareHal(QObject *parent) : ILifecycleModule(parent) {}

HardwareHal::~HardwareHal() {}

void HardwareHal::onInit() {}

void HardwareHal::onStart() {
  EventBus::getInstance()->subscribe("hal/req/hw/led_toggle", this,
                                     [this](const QVariant &payload) {
                                       Q_UNUSED(payload);
                                       toggleLed();
                                     });
  EventBus::getInstance()->subscribe("hal/req/hw/buzzer_toggle", this,
                                     [this](const QVariant &payload) {
                                       Q_UNUSED(payload);
                                       toggleBuzzer();
                                     });
  EventBus::getInstance()->subscribe(
      "hal/req/hw/backlight_set", this, [this](const QVariant &payload) {
        setBacklight(payload.toInt());
      });

  m_sensorTimer = new QTimer(this);
  m_sensorTimer->setInterval(500);
  connect(m_sensorTimer, &QTimer::timeout, this, &HardwareHal::readSensors);
  m_sensorTimer->start();

  EventBus::getInstance()->publish("hal/pub/hw/led_state",
                                   QVariant::fromValue(m_ledState));
  EventBus::getInstance()->publish("hal/pub/hw/buzzer_state",
                                   QVariant::fromValue(m_buzzerState));
  readSensors();

  qInfo() << "[HardwareHal] Started successfully in thread:"
          << QThread::currentThreadId();
}

void HardwareHal::onStop() {
  if (m_sensorTimer) {
    m_sensorTimer->stop();
    m_sensorTimer->deleteLater();
    m_sensorTimer = nullptr;
  }
  qInfo() << "[HardwareHal] Stopped.";
}

void HardwareHal::writeSysFs(const QString &path, const QString &val) {
  QFile file(path);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&file);
    out << val;
    file.close();
  } else {
    qWarning() << "[HardwareHal] Failed to write sysfs:" << path;
  }
}

QString HardwareHal::readSysFs(const QString &path) {
  QFile file(path);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&file);
    QString data = in.readAll().trimmed();
    file.close();
    return data;
  }
  return QString();
}

void HardwareHal::toggleLed() {
  m_ledState = !m_ledState;
  writeSysFs("/sys/class/leds/sys-led/brightness", m_ledState ? "1" : "0");
  EventBus::getInstance()->publish("hal/pub/hw/led_state",
                                   QVariant::fromValue(m_ledState));
}

void HardwareHal::toggleBuzzer() {
  m_buzzerState = !m_buzzerState;
  writeSysFs("/sys/class/gpio/beep/value", m_buzzerState ? "1" : "0");
  EventBus::getInstance()->publish("hal/pub/hw/buzzer_state",
                                   QVariant::fromValue(m_buzzerState));
}

void HardwareHal::setBacklight(int value) {
  const int safeVal = qMax(1, value);
  writeSysFs("/sys/class/backlight/backlight/brightness",
             QString::number(safeVal));
}

void HardwareHal::readSensors() {
  const QString alsStr = readSysFs("/sys/bus/i2c/devices/1-001e/als");
  if (!alsStr.isEmpty()) {
    EventBus::getInstance()->publish("hal/pub/hw/sensor/als",
                                     QVariant::fromValue(alsStr));
    EventBus::getInstance()->publish("hal/pub/hw/sensor/ps",
                                     QVariant::fromValue(QString("较近")));
    EventBus::getInstance()->publish(
        "hal/pub/hw/sensor/imu",
        QVariant::fromValue(QString("[x:0.0, y:0.0, z:9.8]")));
  } else {
    EventBus::getInstance()->publish(
        "hal/pub/hw/sensor/als",
        QVariant::fromValue(
            QString::number(100 + QRandomGenerator::global()->bounded(50))));
    EventBus::getInstance()->publish("hal/pub/hw/sensor/ps",
                                     QVariant::fromValue(QString("较远")));
    EventBus::getInstance()->publish(
        "hal/pub/hw/sensor/imu",
        QVariant::fromValue(
            QString("[x:%1, y:%2, z:9.8]")
                .arg((QRandomGenerator::global()->bounded(11) - 5) / 10.0)
                .arg((QRandomGenerator::global()->bounded(11) - 5) / 10.0)));
  }
}
