#include "AudioHal.h"
#include <QDebug>
#include <QDir>
#include <QMetaObject>
#include <QThread>
#include <QUrl>
#include <QVariant>

AudioHal::AudioHal(QObject *parent)
    : ILifecycleModule(parent), m_player(nullptr), m_playlist(nullptr) {}

AudioHal::~AudioHal() {}

void AudioHal::onInit() {}

void AudioHal::onStart() {
  m_player = new QMediaPlayer(this);
  m_playlist = new QMediaPlaylist(this);
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

  qInfo() << "[AudioHal] Started successfully in thread:"
          << QThread::currentThreadId();
}

void AudioHal::onStop() {
  if (m_player) {
    m_player->stop();
    m_player->deleteLater();
    m_player = nullptr;
  }
  if (m_playlist) {
    m_playlist->deleteLater();
    m_playlist = nullptr;
  }
  // EventBus 内部利用 connect 实现，对象销毁会自动解绑，不需要显示注销
  qInfo() << "[AudioHal] Stopped.";
}

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
