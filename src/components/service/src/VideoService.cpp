#include "VideoService.h"
#include <QDebug>
#include <QThread>
#include <QVariant>

VideoService::VideoService(QObject *parent)
    : ILifecycleModule(parent), m_currentIndex(-1) {}

VideoService::~VideoService() {}

void VideoService::onInit() {}

void VideoService::onStart() {
  EventBus::getInstance()->subscribe(
      "svc/req/video/scan", this, [this](const QVariant &payload) {
        requestScan(payload.toString());
      });

  EventBus::getInstance()->subscribe(
      "hal/pub/video/list", this, [this](const QVariant &payload) {
        onHalVideoList(payload.toStringList());
      });

  EventBus::getInstance()->subscribe(
      "svc/req/video/play_index", this, [this](const QVariant &payload) {
        handlePlayCommand(payload.toInt());
      });

  EventBus::getInstance()->subscribe("svc/req/video/next", this,
                                     [this](const QVariant &payload) {
                                       Q_UNUSED(payload);
                                       handleNextCommand();
                                     });

  EventBus::getInstance()->subscribe("svc/req/video/prev", this,
                                     [this](const QVariant &payload) {
                                       Q_UNUSED(payload);
                                       handlePrevCommand();
                                     });

  qInfo() << "[VideoService] Started successfully in thread:"
          << QThread::currentThreadId();
}

void VideoService::onStop() { qInfo() << "[VideoService] Stopped."; }

void VideoService::requestScan(const QString &dirPath) {
  m_currentDir = dirPath;
  EventBus::getInstance()->publish("hal/req/video/scan",
                                   QVariant::fromValue(m_currentDir));
}

void VideoService::onHalVideoList(const QStringList &list) {
  m_playlist = list;
  EventBus::getInstance()->publish("svc/pub/video/list",
                                   QVariant::fromValue(m_playlist));

  if (m_currentIndex >= m_playlist.size()) {
    m_currentIndex = -1;
  }
}

void VideoService::handlePlayCommand(int index) {
  if (index >= 0 && index < m_playlist.size()) {
    m_currentIndex = index;
    notifyPlay();
  }
}

void VideoService::handleNextCommand() {
  if (m_playlist.isEmpty())
    return;

  if (m_currentIndex < 0) {
    m_currentIndex = 0;
  } else {
    m_currentIndex = (m_currentIndex + 1) % m_playlist.size();
  }
  notifyPlay();
}

void VideoService::handlePrevCommand() {
  if (m_playlist.isEmpty())
    return;

  if (m_currentIndex < 0) {
    m_currentIndex = 0;
  } else {
    m_currentIndex = (m_currentIndex - 1 + m_playlist.size()) % m_playlist.size();
  }
  notifyPlay();
}

void VideoService::notifyPlay() {
  if (m_currentIndex < 0 || m_currentIndex >= m_playlist.size())
    return;

  const QString filename = m_playlist.at(m_currentIndex);
  const QString filePath = QDir(m_currentDir).absoluteFilePath(filename);

  EventBus::getInstance()->publish("svc/pub/video/current_index",
                                   QVariant::fromValue(m_currentIndex));
  EventBus::getInstance()->publish("svc/pub/video/play_path",
                                   QVariant::fromValue(filePath));
}
