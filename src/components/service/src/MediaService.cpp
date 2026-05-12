#include "MediaService.h"
#include <QDebug>
#include <QRandomGenerator>
#include <QThread>
#include <QUrl>
#include <QVariant>

MediaService::MediaService(QObject *parent)
    : ILifecycleModule(parent), m_currentIndex(-1) {}

MediaService::~MediaService() {}

void MediaService::onInit() {}

void MediaService::onStart() {
  EventBus::getInstance()->subscribe(
      "svc/req/music/scan", this, [this](const QVariant &payload) {
        scanMusicDir(payload.toString());
      });

  EventBus::getInstance()->subscribe(
      "svc/req/music/play_index", this, [this](const QVariant &payload) {
        handlePlayCommand(payload.toInt());
      });

  EventBus::getInstance()->subscribe(
      "svc/req/music/next", this, [this](const QVariant &payload) {
        handleNextCommand(payload.toInt());
      });

  EventBus::getInstance()->subscribe(
      "svc/req/music/prev", this, [this](const QVariant &payload) {
        handlePrevCommand(payload.toInt());
      });

  qInfo() << "[MediaService] Started successfully in thread:"
          << QThread::currentThreadId();
}

void MediaService::onStop() {
  // 因为 EventBus 是基于 connect 实现，模块销毁时 Qt 会自动清理相关的连接，
  // 此处不需要显式调用 unsubscribe
  qInfo() << "[MediaService] Stopped.";
}

void MediaService::scanMusicDir(const QString &dirPath) {
  m_currentDir = dirPath;
  QDir dir(m_currentDir);
  m_playlist = dir.entryList({"*.mp3", "*.wav"}, QDir::Files);
  EventBus::getInstance()->publish("svc/pub/music/list",
                                   QVariant::fromValue(m_playlist));
}

void MediaService::handlePlayCommand(int index) {
  if (index >= 0 && index < m_playlist.size()) {
    m_currentIndex = index;
    notifyPlay();
  }
}

void MediaService::handleNextCommand(int mode) {
  if (m_playlist.isEmpty())
    return;
  if (mode == 2) { // Random
    m_currentIndex = QRandomGenerator::global()->bounded(m_playlist.size());
  } else if (mode == 1) { // LoopSingle
    if (m_currentIndex < 0)
      m_currentIndex = 0;
  } else { // LoopList or LoopSingle (as next)
    m_currentIndex = (m_currentIndex + 1) % m_playlist.size();
  }
  notifyPlay();
}

void MediaService::handlePrevCommand(int mode) {
  if (m_playlist.isEmpty())
    return;
  if (mode == 2) { // Random
    m_currentIndex = QRandomGenerator::global()->bounded(m_playlist.size());
  } else if (mode == 1) { // LoopSingle
    if (m_currentIndex < 0)
      m_currentIndex = 0;
  } else { // LoopList or LoopSingle (as prev)
    m_currentIndex =
        (m_currentIndex - 1 + m_playlist.size()) % m_playlist.size();
  }
  notifyPlay();
}

void MediaService::togglePlayPause() {
  // UI handles toggle play/pause directly with AudioHal or
  // MediaService passes it.
}

void MediaService::notifyPlay() {
  if (m_currentIndex < 0 || m_currentIndex >= m_playlist.size())
    return;
  QString filename = m_playlist.at(m_currentIndex);
  QString filePath = QDir(m_currentDir).absoluteFilePath(filename);

  EventBus::getInstance()->publish("svc/pub/music/current_song",
                                   QVariant::fromValue(m_currentIndex));
  EventBus::getInstance()->publish("hal/req/audio/play",
                                   QVariant::fromValue(filePath));
}
