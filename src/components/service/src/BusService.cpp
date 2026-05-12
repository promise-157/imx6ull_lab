#include "BusService.h"
#include "LocalAsrService.h"
#include <QDebug>
#include <QMetaObject>
#include <QVariant>

BusService::BusService(QObject *parent)
    : ILifecycleModule(parent), m_asr(nullptr), m_asrThread(nullptr) {}

BusService::~BusService() {
  if (m_asrThread && m_asrThread->isRunning()) {
    m_asrThread->quit();
    m_asrThread->wait();
  }
  if (m_asr) {
    delete m_asr;
    m_asr = nullptr;
  }
}

void BusService::onInit() {}

void BusService::onStart() {
  // 模型与线程延迟到onStart里初始化，此时BusService已被移动到正确的子线程，彻底解决父子跨线程警告
  m_asr = new LocalAsrService();
  m_asrThread = new QThread(this);
  m_asr->moveToThread(m_asrThread);
  m_asrThread->start();

  // 模型路径：请修改为你实际板子上的模型目录（或继续使用你当前的本地路径）
  bool modelRet = false;
  bool invokeRet = QMetaObject::invokeMethod(
      m_asr, "initModel", Qt::BlockingQueuedConnection,
      Q_RETURN_ARG(bool, modelRet), Q_ARG(QString, "/home/root/model"));

  if (invokeRet && modelRet)
    qDebug() << "AI Model initialized successfully on background thread.";
  else
    qDebug() << "AI Model failed to init.";

  // 监听大模型的【阶段性出字（未完全断句）】事件并打印日志，缓解等待焦虑
  connect(m_asr, &LocalAsrService::partialResultReady, this, [](QString text) {
    qDebug() << "------------> [ASR 正在非常努力地识别中...]:" << text;
  });

  // 监听大模型的出字事件并发往UI (QueuedConnection 是自动的，因为 m_asr
  // 在子线程，this 在主线程)
  connect(m_asr, &LocalAsrService::finalResultReady, this,
          [this](QString text) {
            EventBus::getInstance()->publish("svc/pub/bus/asr_result", text);
          });

    // 监听本地录音结束并产生的WAV文件的请求，投递给后台大模型 
    EventBus::getInstance()->subscribe( 
        "svc/req/bus/asr_recognize_wav", this, [this](const QVariant &payload) { 
          QString path = payload.toString(); 
          if (!path.isEmpty()) { 
            QMetaObject::invokeMethod(m_asr, "recognizeWav", Qt::QueuedConnection, 
                                      Q_ARG(QString, path)); 
          } 
        });

  // 【下行：UI发出请求 -> HAL执行】
  EventBus::getInstance()->subscribe(
      "svc/req/bus/uart_open", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("hal/req/bus/uart_open", payload);
      });
  EventBus::getInstance()->subscribe(
      "svc/req/bus/uart_close", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("hal/req/bus/uart_close", payload);
      });
  EventBus::getInstance()->subscribe(
      "svc/req/bus/uart_tx", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("hal/req/bus/uart_tx", payload);
      });

  // I2C
  EventBus::getInstance()->subscribe(
      "svc/req/bus/i2c_read", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("hal/req/bus/i2c_read", payload);
      });
  EventBus::getInstance()->subscribe(
      "svc/req/bus/i2c_write", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("hal/req/bus/i2c_write", payload);
      });

  // SPI
  EventBus::getInstance()->subscribe(
      "svc/req/bus/spi_transfer", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("hal/req/bus/spi_transfer", payload);
      });

  // CAN
  EventBus::getInstance()->subscribe(
      "svc/req/bus/can_up", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("hal/req/bus/can_up", payload);
      });
  EventBus::getInstance()->subscribe(
      "svc/req/bus/can_down", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("hal/req/bus/can_down", payload);
      });
  EventBus::getInstance()->subscribe(
      "svc/req/bus/can_tx", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("hal/req/bus/can_tx", payload);
      });

  // SYSFS
  EventBus::getInstance()->subscribe(
      "svc/req/bus/sysfs_write", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("hal/req/bus/sysfs_write", payload);
      });
  EventBus::getInstance()->subscribe(
      "svc/req/bus/sysfs_read", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("hal/req/bus/sysfs_read", payload);
      });

  // AUDIO
  EventBus::getInstance()->subscribe(
      "svc/req/bus/audio_record_start", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("hal/req/bus/audio_record_start",
                                         payload);
      });
  EventBus::getInstance()->subscribe(
      "svc/req/bus/audio_record_stop", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("hal/req/bus/audio_record_stop",
                                         payload);
      });
  EventBus::getInstance()->subscribe(
      "svc/req/bus/audio_play_start", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("hal/req/bus/audio_play_start",
                                         payload);
      });
  EventBus::getInstance()->subscribe(
      "svc/req/bus/audio_play_stop", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("hal/req/bus/audio_play_stop",
                                         payload);
      });

  // 【上行：HAL发出状态 -> 转发给UI】
  EventBus::getInstance()->subscribe(
      "hal/pub/bus/audio_status", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("svc/pub/bus/audio_status", payload);
      });
  EventBus::getInstance()->subscribe(
      "hal/pub/bus/uart_status", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("svc/pub/bus/uart_status", payload);
      });
  EventBus::getInstance()->subscribe(
      "hal/pub/bus/uart_rx", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("svc/pub/bus/uart_rx", payload);
      });

  EventBus::getInstance()->subscribe(
      "hal/pub/bus/i2c_rx", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("svc/pub/bus/i2c_rx", payload);
      });
  EventBus::getInstance()->subscribe(
      "hal/pub/bus/spi_rx", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("svc/pub/bus/spi_rx", payload);
      });
  EventBus::getInstance()->subscribe(
      "hal/pub/bus/can_rx", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("svc/pub/bus/can_rx", payload);
      });
  EventBus::getInstance()->subscribe(
      "hal/pub/bus/sysfs_read_ret", this, [this](const QVariant &payload) {
        EventBus::getInstance()->publish("svc/pub/bus/sysfs_read_ret", payload);
      });

  qInfo() << "[BusService] Started.";
}

void BusService::onStop() {
  qInfo() << "[BusService] Stopped.";
  if (m_asr) {
    m_asr->stopExiting();
  }
}
