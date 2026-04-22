#include "MediaService.h"
#include <QUrl>
#include <QVariant>

MediaServiceWorker::MediaServiceWorker(QObject *parent)
    : QObject(parent), m_currentIndex(-1) {}

void MediaServiceWorker::scanMusicDir(const QString &dirPath) {
  m_currentDir = dirPath;
  QDir dir(m_currentDir);
  m_playlist = dir.entryList({"*.mp3", "*.wav"}, QDir::Files);
  EventBus::getInstance()->publish("svc/pub/music/list",
                                   QVariant::fromValue(m_playlist));
}

void MediaServiceWorker::handlePlayCommand(int index) {
  if (index >= 0 && index < m_playlist.size()) {
    m_currentIndex = index;
    notifyPlay();
  }
}

void MediaServiceWorker::handleNextCommand(int mode) {
  if (m_playlist.isEmpty())
    return;
  if (mode == 2) { // Random
    m_currentIndex = qrand() % m_playlist.size();
  } else { // LoopList or LoopSingle (as next)
    m_currentIndex = (m_currentIndex + 1) % m_playlist.size();
  }
  notifyPlay();
}

void MediaServiceWorker::handlePrevCommand(int mode) {
  if (m_playlist.isEmpty())
    return;
  if (mode == 2) { // Random
    m_currentIndex = qrand() % m_playlist.size();
  } else { // LoopList or LoopSingle (as prev)
    m_currentIndex =
        (m_currentIndex - 1 + m_playlist.size()) % m_playlist.size();
  }
  notifyPlay();
}

void MediaServiceWorker::togglePlayPause() {
  // We don't keep track of playing state here, we just tell hal to toggle.
  // Wait, let AudioHal handle pause. Here we can just toggle via hal.
  // Actually, UI handles toggle play/pause directly with AudioHal or
  // MediaService passes it.
}

void MediaServiceWorker::notifyPlay() {
  if (m_currentIndex < 0 || m_currentIndex >= m_playlist.size())
    return;
  QString filename = m_playlist.at(m_currentIndex);
  QString filePath = QDir(m_currentDir).absoluteFilePath(filename);

  EventBus::getInstance()->publish("svc/pub/music/current_song",
                                   QVariant::fromValue(m_currentIndex));
  EventBus::getInstance()->publish("hal/req/audio/play",
                                   QVariant::fromValue(filePath));
}

// =======================

MediaService::MediaService(QObject *parent) : QObject(parent) {
  m_thread = new QThread(this);
  m_worker = new MediaServiceWorker();
  m_worker->moveToThread(m_thread);

  // Map EventBus reqs to worker slots via Qt signals to cross threads safely
  // Since EventBus invoke callbacks from whatever thread publishes,
  // we use QMetaObject::invokeMethod.

  EventBus::getInstance()->subscribe(
      "svc/req/music/scan", this, [this](const QVariant &payload) {
        QMetaObject::invokeMethod(m_worker, "scanMusicDir",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, payload.toString()));
      });

  EventBus::getInstance()->subscribe(
      "svc/req/music/play_index", this, [this](const QVariant &payload) {
        QMetaObject::invokeMethod(m_worker, "handlePlayCommand",
                                  Qt::QueuedConnection,
                                  Q_ARG(int, payload.toInt()));
      });

  EventBus::getInstance()->subscribe(
      "svc/req/music/next", this, [this](const QVariant &payload) {
        QMetaObject::invokeMethod(m_worker, "handleNextCommand",
                                  Qt::QueuedConnection,
                                  Q_ARG(int, payload.toInt()));
      });

  EventBus::getInstance()->subscribe(
      "svc/req/music/prev", this, [this](const QVariant &payload) {
        QMetaObject::invokeMethod(m_worker, "handlePrevCommand",
                                  Qt::QueuedConnection,
                                  Q_ARG(int, payload.toInt()));
      });

  m_thread->start();
}

MediaService::~MediaService() {
  m_thread->quit();
  m_thread->wait();
  m_worker->deleteLater();
}