#include "AudioHal.h"
#include <QDir>
#include <QMetaObject>
#include <QUrl>
#include <QVariant>

AudioHal::AudioHal(QObject *parent)
    : QObject(parent), m_player(new QMediaPlayer(this)),
      m_playlist(new QMediaPlaylist(this)) {
  m_player->setPlaylist(m_playlist);

  connect(m_player, &QMediaPlayer::positionChanged, this, [this](qint64 pos) {
    EventBus::getInstance()->publish("hal/pub/audio/position",
                                     QVariant::fromValue(pos));
  });
  connect(m_player, &QMediaPlayer::durationChanged, this, [this](qint64 dur) {
    EventBus::getInstance()->publish("hal/pub/audio/duration",
                                     QVariant::fromValue(dur));
  });
  connect(m_player, &QMediaPlayer::mediaStatusChanged, this,
          [this](QMediaPlayer::MediaStatus status) {
            EventBus::getInstance()->publish(
                "hal/pub/audio/status",
                QVariant::fromValue(static_cast<int>(status)));
          });

  EventBus::getInstance()->subscribe(
      "hal/req/audio/play", this, [this](const QVariant &payload) {
        if (payload.type() == QVariant::String) {
          QMetaObject::invokeMethod(this, "doPlay", Qt::QueuedConnection,
                                    Q_ARG(QString, payload.toString()));
        } else {
          QMetaObject::invokeMethod(this, "doResume", Qt::QueuedConnection);
        }
      });

  EventBus::getInstance()->subscribe(
      "hal/req/audio/pause", this, [this](const QVariant &payload) {
        QMetaObject::invokeMethod(this, "doPause", Qt::QueuedConnection);
      });

  EventBus::getInstance()->subscribe(
      "hal/req/audio/stop", this, [this](const QVariant &payload) {
        QMetaObject::invokeMethod(this, "doStop", Qt::QueuedConnection);
      });

  EventBus::getInstance()->subscribe(
      "hal/req/audio/seek", this, [this](const QVariant &payload) {
        QMetaObject::invokeMethod(this, "doSeek", Qt::QueuedConnection,
                                  Q_ARG(qint64, payload.toLongLong()));
      });
  EventBus::getInstance()->subscribe(
      "hal/req/audio/volume", this, [this](const QVariant &payload) {
        QMetaObject::invokeMethod(this, "doSetVolume", Qt::QueuedConnection,
                                  Q_ARG(int, payload.toInt()));
      });
}

AudioHal::~AudioHal() {}

void AudioHal::doPlay(const QString &urlStr) {
  m_playlist->clear();
  m_playlist->addMedia(QUrl::fromLocalFile(urlStr));
  m_playlist->setCurrentIndex(0);
  m_player->play();
}

void AudioHal::doResume() { m_player->play(); }

void AudioHal::doPause() { m_player->pause(); }

void AudioHal::doStop() { m_player->stop(); }

void AudioHal::doSeek(qint64 pos) { m_player->setPosition(pos); }
void AudioHal::doSetVolume(int vol) { m_player->setVolume(vol); }
