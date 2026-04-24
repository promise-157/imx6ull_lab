#include "VideoHal.h"
#include <QDebug>
#include <QDir>
#include <QThread>
#include <QVariant>

VideoHal::VideoHal(QObject *parent) : ILifecycleModule(parent) {}

VideoHal::~VideoHal() {}

void VideoHal::onInit() {}

void VideoHal::onStart() {
  EventBus::getInstance()->subscribe(
      "hal/req/video/scan", this, [this](const QVariant &payload) {
        scanVideoDir(payload.toString());
      });

  qInfo() << "[VideoHal] Started successfully in thread:"
          << QThread::currentThreadId();
}

void VideoHal::onStop() { qInfo() << "[VideoHal] Stopped."; }

void VideoHal::scanVideoDir(const QString &dirPath) {
  QDir dir(dirPath);
  QStringList playlist = dir.entryList({"*.mp4", "*.avi", "*.mkv"}, QDir::Files);
  EventBus::getInstance()->publish("hal/pub/video/list",
                                   QVariant::fromValue(playlist));
}
