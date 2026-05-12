#ifndef BUSHAL_H
#define BUSHAL_H

#include "EventBus.h"
#include "ILifecycleModule.h"
#include <QTimer>

class LinuxUart;
class LinuxI2c;
class LinuxSpi;
class SocketCan;
class LinuxAlsaAudio;

class BusHal : public ILifecycleModule {
  Q_OBJECT
public:
  explicit BusHal(QObject *parent = nullptr);
  ~BusHal() override;

  QString moduleName() const override { return "BusHal"; }

protected:
  void onInit() override;
  void onStart() override;
  void onStop() override;

private slots:
  void pollUart();
  void pollCan();

private:
  LinuxUart *m_uartDev;
  QTimer *m_uartTimer;

  LinuxI2c *m_i2cDev;
  LinuxSpi *m_spiDev;
  SocketCan *m_canDev;
  QTimer *m_canTimer;

  LinuxAlsaAudio *m_alsaAudio;
};

#endif // BUSHAL_H
