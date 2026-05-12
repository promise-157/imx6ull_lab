#ifndef BUSSERVICE_H
#define BUSSERVICE_H

#include "EventBus.h"
#include "ILifecycleModule.h"
#include <QThread>

class LocalAsrService;

class BusService : public ILifecycleModule {
  Q_OBJECT
public:
  explicit BusService(QObject *parent = nullptr);
  ~BusService() override;

  QString moduleName() const override { return "BusService"; }

protected:
  void onInit() override;
  void onStart() override;
  void onStop() override;

private:
  LocalAsrService *m_asr;
  QThread *m_asrThread;
};

#endif // BUSSERVICE_H
