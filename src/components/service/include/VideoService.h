#ifndef VIDEO_SERVICE_H
#define VIDEO_SERVICE_H

#include "EventBus.h"
#include "ILifecycleModule.h"
#include <QDir>
#include <QObject>
#include <QStringList>

class VideoService : public ILifecycleModule {
  Q_OBJECT
public:
  explicit VideoService(QObject *parent = nullptr);
  ~VideoService() override;

  QString moduleName() const override { return "VideoService"; }
  void onInit() override;
  void onStart() override;
  void onStop() override;

public slots:
  void requestScan(const QString &dirPath);
  void onHalVideoList(const QStringList &list);
  void handlePlayCommand(int index);
  void handleNextCommand();
  void handlePrevCommand();

private:
  QString m_currentDir;
  QStringList m_playlist;
  int m_currentIndex;

  void notifyPlay();
};

#endif // VIDEO_SERVICE_H
