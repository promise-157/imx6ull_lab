#include "HardwareService.h"
#include <QDebug>
#include <QThread>
#include <QVariant>

HardwareService::HardwareService(QObject *parent) : ILifecycleModule(parent) {}

HardwareService::~HardwareService() {}

void HardwareService::onInit() {}

void HardwareService::onStart() {
  EventBus::getInstance()->subscribe("svc/req/hw/led_toggle", this,
                                     [this](const QVariant &payload) {
                                       Q_UNUSED(payload);
                                       toggleLed();
                                     });
  EventBus::getInstance()->subscribe("svc/req/hw/buzzer_toggle", this,
                                     [this](const QVariant &payload) {
                                       Q_UNUSED(payload);
                                       toggleBuzzer();
                                     });
  EventBus::getInstance()->subscribe(
      "svc/req/hw/backlight_set", this, [this](const QVariant &payload) {
        setBacklight(payload.toInt());
      });

  EventBus::getInstance()->subscribe(
      "hal/pub/hw/led_state", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("svc/pub/hw/led_state", payload);
      });
  EventBus::getInstance()->subscribe(
      "hal/pub/hw/buzzer_state", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("svc/pub/hw/buzzer_state", payload);
      });
  EventBus::getInstance()->subscribe(
      "hal/pub/hw/sensor/als", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("svc/pub/hw/sensor/als", payload);
      });
  EventBus::getInstance()->subscribe(
      "hal/pub/hw/sensor/ps", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("svc/pub/hw/sensor/ps", payload);
      });
  EventBus::getInstance()->subscribe(
      "hal/pub/hw/sensor/imu", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("svc/pub/hw/sensor/imu", payload);
      });

  qInfo() << "[HardwareService] Started successfully in thread:"
          << QThread::currentThreadId();
}

void HardwareService::onStop() { qInfo() << "[HardwareService] Stopped."; }

void HardwareService::toggleLed() {
  EventBus::getInstance()->publish("hal/req/hw/led_toggle");
}

void HardwareService::toggleBuzzer() {
  EventBus::getInstance()->publish("hal/req/hw/buzzer_toggle");
}

void HardwareService::setBacklight(int value) {
  EventBus::getInstance()->publish("hal/req/hw/backlight_set",
                                   QVariant::fromValue(value));
}
