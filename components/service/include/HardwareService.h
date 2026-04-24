#ifndef HARDWARE_SERVICE_H
#define HARDWARE_SERVICE_H

#include "EventBus.h"
#include "ILifecycleModule.h"
#include <QObject>

class HardwareService : public ILifecycleModule {
  Q_OBJECT
public:
  explicit HardwareService(QObject *parent = nullptr);
  ~HardwareService() override;

  QString moduleName() const override { return "HardwareService"; }
  void onInit() override;
  void onStart() override;
  void onStop() override;

public slots:
  void toggleLed();
  void toggleBuzzer();
  void setBacklight(int value);
};

#endif // HARDWARE_SERVICE_H
