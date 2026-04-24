#ifndef MEDIA_SERVICE_H
#define MEDIA_SERVICE_H

#include "EventBus.h"
#include "ILifecycleModule.h"
#include <QDir>
#include <QObject>
#include <QStringList>

class MediaService : public ILifecycleModule {
  Q_OBJECT
public:
  explicit MediaService(QObject *parent = nullptr);
  ~MediaService();

  QString moduleName() const override { return "MediaService"; }
  void onInit() override;
  void onStart() override;
  void onStop() override;

public slots:
  void scanMusicDir(const QString &dirPath);
  void handlePlayCommand(int index);
  void handleNextCommand(int mode);
  void handlePrevCommand(int mode);
  void togglePlayPause();

private:
  QString m_currentDir;
  QStringList m_playlist;
  int m_currentIndex;

  void notifyPlay();
};

#endif // MEDIA_SERVICE_H