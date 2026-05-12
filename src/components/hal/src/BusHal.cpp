#include "BusHal.h"
#include "bus/LinuxAlsaAudio.h"
#include "bus/LinuxI2c.h"
#include "bus/LinuxSpi.h"
#include "bus/LinuxUart.h"
#include "bus/SocketCan.h"
#include <QDebug>
#include <QVariant>

BusHal::BusHal(QObject *parent)
    : ILifecycleModule(parent), m_uartDev(new LinuxUart()),
      m_uartTimer(nullptr), m_i2cDev(new LinuxI2c()), m_spiDev(new LinuxSpi()),
      m_canDev(new SocketCan()), m_canTimer(nullptr),
      m_alsaAudio(new LinuxAlsaAudio()) {}

BusHal::~BusHal() {
  delete m_uartDev;
  delete m_i2cDev;
  delete m_spiDev;
  delete m_canDev;
  delete m_alsaAudio;
}

void BusHal::onInit() {}

void BusHal::onStart() {
  // 监听：服务层发来的 打开串口 请求
  EventBus::getInstance()->subscribe(
      "hal/req/bus/uart_open", this, [this](const QVariant &payload) {
        QMap<QString, QVariant> params = payload.toMap();
        QString dev = params.value("dev", "/dev/ttymxc2").toString();
        int baud = params.value("baud", 115200).toInt();
        bool ok = m_uartDev->openDevice(dev, baud);
        EventBus::getInstance()->publish("hal/pub/bus/uart_status", ok);
      });

  // 监听：服务层发来的 关闭串口 请求
  EventBus::getInstance()->subscribe(
      "hal/req/bus/uart_close", this, [this](const QVariant &payload) {
        Q_UNUSED(payload);
        m_uartDev->closeDevice();
        EventBus::getInstance()->publish("hal/pub/bus/uart_status", false);
      });

  // 监听：服务层发来的 串口发送 请求
  EventBus::getInstance()->subscribe(
      "hal/req/bus/uart_tx", this, [this](const QVariant &payload) {
        if (m_uartDev->isOpen()) {
          m_uartDev->writeData(payload.toByteArray());
        }
      });

  // I2C
  EventBus::getInstance()->subscribe(
      "hal/req/bus/i2c_read", this, [this](const QVariant &payload) {
        QMap<QString, QVariant> map = payload.toMap();
        m_i2cDev->openDevice(map["dev"].toString(), map["addr"].toInt());
        if (m_i2cDev->isOpen()) {
          QByteArray rx =
              m_i2cDev->readData(map["reg"].toInt(), map["len"].toInt());
          EventBus::getInstance()->publish("hal/pub/bus/i2c_rx", rx);
          m_i2cDev->closeDevice();
        }
      });
  EventBus::getInstance()->subscribe(
      "hal/req/bus/i2c_write", this, [this](const QVariant &payload) {
        QMap<QString, QVariant> map = payload.toMap();
        m_i2cDev->openDevice(map["dev"].toString(), map["addr"].toInt());
        if (m_i2cDev->isOpen()) {
          m_i2cDev->writeData(map["reg"].toInt(), map["data"].toByteArray());
          m_i2cDev->closeDevice();
        }
      });

  // SPI
  EventBus::getInstance()->subscribe(
      "hal/req/bus/spi_transfer", this, [this](const QVariant &payload) {
        QMap<QString, QVariant> map = payload.toMap();
        m_spiDev->openDevice(map["dev"].toString(), map["mode"].toInt(),
                             map["speed"].toInt(), 8);
        if (m_spiDev->isOpen()) {
          QByteArray rx = m_spiDev->transfer(map["data"].toByteArray());
          EventBus::getInstance()->publish("hal/pub/bus/spi_rx", rx);
          m_spiDev->closeDevice();
        }
      });

  // CAN
  EventBus::getInstance()->subscribe(
      "hal/req/bus/can_up", this, [this](const QVariant &payload) {
        QMap<QString, QVariant> map = payload.toMap();
        m_canDev->openDevice(map["dev"].toString());
        // 波特率设置等通常通过 ip link set 命令设置，SocketCAN应用层直连socket
      });
  EventBus::getInstance()->subscribe("hal/req/bus/can_down", this,
                                     [this](const QVariant &payload) {
                                       Q_UNUSED(payload);
                                       m_canDev->closeDevice();
                                     });
  EventBus::getInstance()->subscribe(
      "hal/req/bus/can_tx", this, [this](const QVariant &payload) {
        if (m_canDev->isOpen()) {
          QStringList parts = payload.toString().split("#");
          if (parts.size() == 2) {
            uint32_t id = parts[0].toUInt(nullptr, 16);
            QByteArray data = QByteArray::fromHex(parts[1].toLocal8Bit());
            m_canDev->writeFrame(id, data);
          }
        }
      });

  // SYSFS
  EventBus::getInstance()->subscribe(
      "hal/req/bus/sysfs_write", this, [this](const QVariant &payload) {
        QMap<QString, QVariant> map = payload.toMap();
        QString path = map["path"].toString();
        QString val = map["val"].toString();
        FILE *f = fopen(path.toLocal8Bit().constData(), "w");
        if (f) {
          fprintf(f, "%s", val.toLocal8Bit().constData());
          fclose(f);
        }
      });

  EventBus::getInstance()->subscribe(
      "hal/req/bus/sysfs_read", this, [this](const QVariant &payload) {
        QString path = payload.toString();
        FILE *f = fopen(path.toLocal8Bit().constData(), "r");
        if (f) {
          char buf[64] = {0};
          if (fgets(buf, sizeof(buf), f) != nullptr) {
            QMap<QString, QVariant> ret;
            ret["path"] = path;
            ret["val"] = QString(buf).trimmed();
            EventBus::getInstance()->publish("hal/pub/bus/sysfs_read_ret", ret);
          }
          fclose(f);
        }
      });

  // AUDIO
  m_alsaAudio->setStatusCallback([](const std::string &msg, bool isFinished) {
    QMap<QString, QVariant> ret;
    ret["msg"] = QString::fromStdString(msg);
    ret["isFinished"] = isFinished;
    EventBus::getInstance()->publish("hal/pub/bus/audio_status", ret);
  });

  // 我们不再用事件流实时向UI抛送PCM数据，改为本地一整段录完解析
  // m_alsaAudio->setAudioDataCallback([](const char *data, int bytes) {
  //   QByteArray arr(data, bytes);
  //   EventBus::getInstance()->publish("hal/pub/bus/audio_data", arr);
  // });

  EventBus::getInstance()->subscribe(
      "hal/req/bus/audio_record_start", this, [this](const QVariant &payload) {
        QString path = payload.toString();
        m_alsaAudio->startRecord(path.toStdString());
      });

  EventBus::getInstance()->subscribe("hal/req/bus/audio_record_stop", this,
                                     [this](const QVariant &payload) {
                                       Q_UNUSED(payload);
                                       m_alsaAudio->stopRecord();
                                     });

  EventBus::getInstance()->subscribe(
      "hal/req/bus/audio_play_start", this, [this](const QVariant &payload) {
        QString path = payload.toString();
        m_alsaAudio->startPlayback(path.toStdString());
      });

  EventBus::getInstance()->subscribe("hal/req/bus/audio_play_stop", this,
                                     [this](const QVariant &payload) {
                                       Q_UNUSED(payload);
                                       m_alsaAudio->stopPlayback();
                                     });

  // 定时轮询读取 UART 缓冲数据并发布出去 (也可用 QSocketNotifier 来避免轮询)
  m_uartTimer = new QTimer(this);
  m_uartTimer->setInterval(50);
  connect(m_uartTimer, &QTimer::timeout, this, &BusHal::pollUart);
  m_uartTimer->start();

  // CAN 轮询
  m_canTimer = new QTimer(this);
  m_canTimer->setInterval(50);
  connect(m_canTimer, &QTimer::timeout, this, &BusHal::pollCan);
  m_canTimer->start();

  qInfo() << "[BusHal] Started.";
}

void BusHal::onStop() {
  if (m_uartTimer) {
    m_uartTimer->stop();
    m_uartTimer->deleteLater();
    m_uartTimer = nullptr;
  }
  if (m_canTimer) {
    m_canTimer->stop();
    m_canTimer->deleteLater();
    m_canTimer = nullptr;
  }

  m_uartDev->closeDevice();
  m_i2cDev->closeDevice();
  m_spiDev->closeDevice();
  m_canDev->closeDevice();
  qInfo() << "[BusHal] Stopped.";
}

void BusHal::pollUart() {
  if (m_uartDev && m_uartDev->isOpen()) {
    QByteArray rx = m_uartDev->readData();
    if (!rx.isEmpty()) {
      EventBus::getInstance()->publish("hal/pub/bus/uart_rx", rx);
    }
  }
}

void BusHal::pollCan() {
  if (m_canDev && m_canDev->isOpen()) {
    uint32_t canId = 0;
    QByteArray rx;
    while (m_canDev->readFrame(canId, rx)) {
      QString msg = QString("%1#%2")
                        .arg(canId, 3, 16, QChar('0'))
                        .arg(QString(rx.toHex(' ').toUpper()));
      EventBus::getInstance()->publish("hal/pub/bus/can_rx", msg);
    }
  }
}
