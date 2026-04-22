#ifndef MEDIA_SERVICE_H
#define MEDIA_SERVICE_H

#include "EventBus.h"
#include <QDir>
#include <QObject>
#include <QStringList>
#include <QThread>

class MediaServiceWorker : public QObject {
  Q_OBJECT
public:
  explicit MediaServiceWorker(QObject *parent = nullptr);

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

class MediaService : public QObject {
  Q_OBJECT
public:
  explicit MediaService(QObject *parent = nullptr);
  ~MediaService();

private:
  QThread *m_thread;
  MediaServiceWorker *m_worker;
};

#endif // MEDIA_SERVICE_H