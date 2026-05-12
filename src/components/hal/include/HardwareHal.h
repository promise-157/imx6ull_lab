#ifndef HARDWARE_HAL_H
#define HARDWARE_HAL_H

#include "EventBus.h"
#include "ILifecycleModule.h"
#include <QObject>
#include <QTimer>

class HardwareHal : public ILifecycleModule {
  Q_OBJECT
public:
  explicit HardwareHal(QObject *parent = nullptr);
  ~HardwareHal() override;

  QString moduleName() const override { return "HardwareHal"; }
  void onInit() override;
  void onStart() override;
  void onStop() override;

public slots:
  void toggleLed();
  void toggleBuzzer();
  void setBacklight(int value);
  void readSensors();

private:
  bool m_ledState = false;
  bool m_buzzerState = false;
  QTimer *m_sensorTimer = nullptr;

  void writeSysFs(const QString &path, const QString &val);
  QString readSysFs(const QString &path);
};

#endif // HARDWARE_HAL_H
