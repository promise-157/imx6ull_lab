#ifndef VIDEO_HAL_H
#define VIDEO_HAL_H

#include "EventBus.h"
#include "ILifecycleModule.h"
#include <QObject>

class VideoHal : public ILifecycleModule {
  Q_OBJECT
public:
  explicit VideoHal(QObject *parent = nullptr);
  ~VideoHal() override;

  QString moduleName() const override { return "VideoHal"; }
  void onInit() override;
  void onStart() override;
  void onStop() override;

public slots:
  void scanVideoDir(const QString &dirPath);
};

#endif // VIDEO_HAL_H
