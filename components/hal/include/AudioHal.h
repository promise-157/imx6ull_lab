#ifndef AUDIO_HAL_H
#define AUDIO_HAL_H

#include "EventBus.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QObject>

class AudioHal : public QObject {
  Q_OBJECT
public:
  explicit AudioHal(QObject *parent = nullptr);
  ~AudioHal();

public slots:
  void doPlay(const QString &urlStr);
  void doResume();
  void doPause();
  void doStop();
  void doSeek(qint64 pos);
  void doSetVolume(int vol);

private:
  QMediaPlayer *m_player;
  QMediaPlaylist *m_playlist;
};

#endif // AUDIO_HAL_H