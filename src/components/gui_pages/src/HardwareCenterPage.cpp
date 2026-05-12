#include "HardwareCenterPage.h"
#include "EventBus.h"
#include <QDebug>
#include <QFile>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QStyle>
#include <QTabWidget>
#include <QVBoxLayout>

HardwareCenterPage::HardwareCenterPage(QWidget *parent) : IAppModule(parent) {
  this->setAttribute(Qt::WA_StyledBackground, true);
  this->setObjectName("HardwareCenterPage");

  QFile file(":/res/style/HardwareCenterPage.qss");
  if (file.open(QFile::ReadOnly)) {
    this->setStyleSheet(QLatin1String(file.readAll()));
    file.close();
  }

  setupUI();
}

HardwareCenterPage::~HardwareCenterPage() { stopService(); }

void HardwareCenterPage::stopService() {}

void HardwareCenterPage::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(10);

  // Header
  QHBoxLayout *topLayout = new QHBoxLayout();
  QLabel *titleLabel =
      new QLabel(QString::fromUtf8("IMX6ULL 多功能外设诊断台"));
  titleLabel->setObjectName("HwTitleLabel");

  QPushButton *btnHome = new QPushButton(QString::fromUtf8("返回"));
  btnHome->setObjectName("HwBtnHome");
  btnHome->setFixedSize(60, 40);
  connect(btnHome, &QPushButton::clicked, this, &IAppModule::requestClose);

  topLayout->addWidget(titleLabel);
  topLayout->addStretch();
  topLayout->addWidget(btnHome);

  mainLayout->addLayout(topLayout);

  // Splitter for Sidebar + Content
  QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

  m_menuTree = new QTreeWidget(this);
  m_menuTree->setHeaderHidden(true);
  m_menuTree->setObjectName("HwMenuTree");

  QTreeWidgetItem *itemIO = new QTreeWidgetItem(
      m_menuTree, QStringList(QString::fromUtf8("外设管脚状态")));
  QTreeWidgetItem *itemBus = new QTreeWidgetItem(
      m_menuTree, QStringList(QString::fromUtf8("串行通信总线")));
  QTreeWidgetItem *itemSys = new QTreeWidgetItem(
      m_menuTree, QStringList(QString::fromUtf8("系统资源监视")));
  QTreeWidgetItem *itemAudio = new QTreeWidgetItem(
      m_menuTree, QStringList(QString::fromUtf8("录音/播放测试")));
  QTreeWidgetItem *itemAnalyzer = new QTreeWidgetItem(
      m_menuTree, QStringList(QString::fromUtf8("逻辑波形分析")));

  m_contentStack = new QStackedWidget(this);
  m_contentStack->addWidget(new BasicIoWidget(this));
  m_contentStack->addWidget(new BusTestWidget(this));
  m_contentStack->addWidget(new SysLoadWidget(this));
  m_contentStack->addWidget(new AudioTestWidget(this));
  m_contentStack->addWidget(new LogicAnalyzerView(this));

  connect(m_menuTree, &QTreeWidget::currentItemChanged, this,
          [this](QTreeWidgetItem *current, QTreeWidgetItem *previous) {
            if (!current)
              return;
            int index = m_menuTree->indexOfTopLevelItem(current);
            if (index >= 0)
              m_contentStack->setCurrentIndex(index);
          });

  m_menuTree->setCurrentItem(itemBus);

  splitter->addWidget(m_menuTree);
  splitter->addWidget(m_contentStack);
  splitter->setSizes({200, 600});

  mainLayout->addWidget(splitter);
}

// --- BasicIoWidget ---
BasicIoWidget::BasicIoWidget(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);

  QGroupBox *ledGroup = new QGroupBox("板载状态指示 (LED & Beep)");
  QHBoxLayout *ledLayout = new QHBoxLayout(ledGroup);

  QPushButton *sysLedBtn = new QPushButton("系统指示灯 (sys-led)");
  sysLedBtn->setCheckable(true);
  QPushButton *beepBtn = new QPushButton("有源蜂鸣器 (beep)");
  beepBtn->setCheckable(true);

  ledLayout->addWidget(sysLedBtn);
  ledLayout->addWidget(beepBtn);
  layout->addWidget(ledGroup);

  connect(sysLedBtn, &QPushButton::toggled, this, [](bool checked) {
    QVariantMap req;
    req["path"] = "/sys/class/leds/sys-led/brightness";
    req["val"] = checked ? "1" : "0";
    EventBus::getInstance()->publish("svc/req/bus/sysfs_write", req);
  });

  connect(beepBtn, &QPushButton::toggled, this, [](bool checked) {
    QVariantMap req;
    req["path"] = "/sys/class/leds/beep/brightness";
    req["val"] = checked ? "1" : "0";
    EventBus::getInstance()->publish("svc/req/bus/sysfs_write", req);
  });

  QGroupBox *thermalGroup = new QGroupBox("板载核心传感器");
  QVBoxLayout *thermalLayout = new QVBoxLayout(thermalGroup);
  QLabel *tempLabel = new QLabel("CPU温度: -- ℃");
  QPushButton *tempBtn = new QPushButton("刷新温度");

  thermalLayout->addWidget(tempLabel);
  thermalLayout->addWidget(tempBtn);
  layout->addWidget(thermalGroup);

  connect(tempBtn, &QPushButton::clicked, this, []() {
    EventBus::getInstance()->publish("svc/req/bus/sysfs_read",
                                     "/sys/class/hwmon/hwmon0/temp1_input");
  });

  EventBus::getInstance()->subscribe(
      "svc/pub/bus/sysfs_read_ret", this, [tempLabel](const QVariant &payload) {
        QMap<QString, QVariant> map = payload.toMap();
        if (map["path"].toString() == "/sys/class/hwmon/hwmon0/temp1_input") {
          float temp = map["val"].toString().toFloat() / 1000.0;
          tempLabel->setText(QString("CPU温度: %1 ℃").arg(temp, 0, 'f', 2));
        }
      });

  layout->addStretch();
}

// --- BusTestWidget ---
BusTestWidget::BusTestWidget(QWidget *parent) : QWidget(parent) { setupUI(); }

void BusTestWidget::setupUI() {
  QVBoxLayout *layout = new QVBoxLayout(this);
  QTabWidget *tabs = new QTabWidget(this);
  tabs->setObjectName("BusTestTabs");

  QWidget *uartTab = new QWidget();
  QWidget *i2cTab = new QWidget();
  QWidget *spiTab = new QWidget();
  QWidget *canTab = new QWidget();

  setupUartTab(uartTab);
  setupI2cTab(i2cTab);
  setupSpiTab(spiTab);
  setupCanTab(canTab);

  tabs->addTab(uartTab, "UART(串口)");
  tabs->addTab(i2cTab, "I2C(微型总线)");
  tabs->addTab(spiTab, "SPI(高速同步)");
  tabs->addTab(canTab, "CAN(工控网络)");

  layout->addWidget(tabs);
}

void BusTestWidget::setupUartTab(QWidget *tab) {
  QVBoxLayout *layout = new QVBoxLayout(tab);

  // Ctrl Layout
  QHBoxLayout *ctrlLayout = new QHBoxLayout();

  ctrlLayout->addWidget(new QLabel("UART节点:"));
  QComboBox *m_uartDevCb = new QComboBox();
  m_uartDevCb->setObjectName("UartDevCb");
  m_uartDevCb->addItems({"/dev/ttymxc2", "/dev/ttymxc0"});
  ctrlLayout->addWidget(m_uartDevCb);

  ctrlLayout->addWidget(new QLabel("波特率(Baud):"));
  m_uartBaudCb = new QComboBox();
  m_uartBaudCb->addItems({"9600", "115200"});
  ctrlLayout->addWidget(m_uartBaudCb);

  m_uartOpenBtn = new QPushButton(QString::fromUtf8("打开串口"));
  m_uartOpenBtn->setObjectName("UartOpenBtn");

  m_uartCloseBtn = new QPushButton(QString::fromUtf8("关闭串口"));
  m_uartCloseBtn->setObjectName("UartCloseBtn");
  m_uartCloseBtn->setEnabled(false);

  connect(m_uartOpenBtn, &QPushButton::clicked, this,
          &BusTestWidget::onUartOpen);
  connect(m_uartCloseBtn, &QPushButton::clicked, this,
          &BusTestWidget::onUartClose);

  ctrlLayout->addStretch();
  ctrlLayout->addWidget(m_uartOpenBtn);
  ctrlLayout->addWidget(m_uartCloseBtn);

  layout->addLayout(ctrlLayout);

  // Output Box
  QGroupBox *rxGroup = new QGroupBox(QString::fromUtf8("接收数据(RX)"));
  QVBoxLayout *rxBox = new QVBoxLayout(rxGroup);
  m_uartRxEdit = new QTextEdit();
  m_uartRxEdit->setReadOnly(true);
  rxBox->addWidget(m_uartRxEdit);

  layout->addWidget(rxGroup, 2);

  // Input Box
  QGroupBox *txGroup = new QGroupBox(QString::fromUtf8("发送数据(TX)"));
  QHBoxLayout *txBox = new QHBoxLayout(txGroup);
  m_uartTxEdit = new QTextEdit();
  m_uartTxEdit->setMaximumHeight(80);

  QVBoxLayout *txBtnLayout = new QVBoxLayout();
  m_uartSendBtn = new QPushButton(QString::fromUtf8(" 发送(Send)"));
  m_uartSendBtn->setObjectName("UartSendBtn");
  connect(m_uartSendBtn, &QPushButton::clicked, this,
          &BusTestWidget::onUartSend);

  QPushButton *clearBtn = new QPushButton(QString::fromUtf8(" 清空(Clear)"));
  clearBtn->setObjectName("UartClearBtn");
  connect(clearBtn, &QPushButton::clicked, m_uartRxEdit, &QTextEdit::clear);

  txBtnLayout->addWidget(m_uartSendBtn);
  txBtnLayout->addWidget(clearBtn);

  txBox->addWidget(m_uartTxEdit, 1);
  txBox->addLayout(txBtnLayout);

  layout->addWidget(txGroup, 1);

  // Subscribe to EventBus
  EventBus::getInstance()->subscribe(
      "svc/pub/bus/uart_rx", this, [this](const QVariant &payload) {
        QByteArray data = payload.toByteArray();
        m_uartRxEdit->append(QString::fromLocal8Bit(data));
      });
}

void BusTestWidget::onUartOpen() {
  QComboBox *devCb = this->findChild<QComboBox *>("UartDevCb");
  QString dev = devCb ? devCb->currentText() : "/dev/ttymxc2";

  QVariantMap req;
  req["dev"] = dev;
  req["baud"] = m_uartBaudCb->currentText().toInt();

  EventBus::getInstance()->publish("svc/req/bus/uart_open", req);

  m_uartOpenBtn->setEnabled(false);
  m_uartCloseBtn->setEnabled(true);
}

void BusTestWidget::onUartClose() {
  EventBus::getInstance()->publish("svc/req/bus/uart_close");
  m_uartOpenBtn->setEnabled(true);
  m_uartCloseBtn->setEnabled(false);
}

void BusTestWidget::onUartSend() {
  QString txt = m_uartTxEdit->toPlainText();
  if (txt.isEmpty())
    return;
  EventBus::getInstance()->publish("svc/req/bus/uart_tx",
                                   QVariant::fromValue(txt.toLocal8Bit()));
}

void BusTestWidget::setupI2cTab(QWidget *tab) {
  QVBoxLayout *layout = new QVBoxLayout(tab);
  QHBoxLayout *ctrlLayout = new QHBoxLayout();

  ctrlLayout->addWidget(new QLabel("I2C节点:"));
  m_i2cDevCb = new QComboBox();
  m_i2cDevCb->addItems({"/dev/i2c-0", "/dev/i2c-1"});
  ctrlLayout->addWidget(m_i2cDevCb);

  ctrlLayout->addWidget(new QLabel("设备地址(Hex):"));
  m_i2cAddrCb = new QComboBox();
  m_i2cAddrCb->addItems(
      {"0x1E", "0x1A", "0x38", "0x3C",
       "0x5D"}); // 对应 ap3216c, wm8960, ft5306, ov5640, gt9xx
  m_i2cAddrCb->setEditable(true);
  ctrlLayout->addWidget(m_i2cAddrCb);

  ctrlLayout->addWidget(new QLabel("寄存器(Hex):"));
  m_i2cRegCb = new QComboBox();
  m_i2cRegCb->addItems({"0x00", "0x01"});
  m_i2cRegCb->setEditable(true);
  ctrlLayout->addWidget(m_i2cRegCb);

  m_i2cReadBtn = new QPushButton("读取(Read)");
  m_i2cWriteBtn = new QPushButton("写入(Write)");

  connect(m_i2cReadBtn, &QPushButton::clicked, this, &BusTestWidget::onI2cRead);
  connect(m_i2cWriteBtn, &QPushButton::clicked, this,
          &BusTestWidget::onI2cWrite);

  ctrlLayout->addStretch();
  ctrlLayout->addWidget(m_i2cReadBtn);
  ctrlLayout->addWidget(m_i2cWriteBtn);

  layout->addLayout(ctrlLayout);

  QGroupBox *dataGroup = new QGroupBox("数据(Data)");
  QVBoxLayout *dataBox = new QVBoxLayout(dataGroup);
  m_i2cDataBox = new QTextEdit();
  m_i2cDataBox->setPlaceholderText("写入时输入Hex数据，读取时此处显示Hex返回");
  dataBox->addWidget(m_i2cDataBox);
  layout->addWidget(dataGroup);

  EventBus::getInstance()->subscribe(
      "svc/pub/bus/i2c_rx", this, [this](const QVariant &payload) {
        QByteArray data = payload.toByteArray();
        m_i2cDataBox->append(
            QString("读回: %1").arg(QString(data.toHex(' ').toUpper())));
      });
}

void BusTestWidget::setupSpiTab(QWidget *tab) {
  QVBoxLayout *layout = new QVBoxLayout(tab);
  QHBoxLayout *ctrlLayout = new QHBoxLayout();

  ctrlLayout->addWidget(new QLabel("SPI节点:"));
  m_spiDevCb = new QComboBox();
  m_spiDevCb->addItems({"/dev/spidev2.0", "/dev/spidev0.0", "/dev/spidev0.1"});
  ctrlLayout->addWidget(m_spiDevCb);

  ctrlLayout->addWidget(new QLabel("工作模式:"));
  m_spiModeCb = new QComboBox();
  m_spiModeCb->addItems({"Mode 0", "Mode 1", "Mode 2", "Mode 3"});
  ctrlLayout->addWidget(m_spiModeCb);

  ctrlLayout->addWidget(new QLabel("速率(Hz):"));
  m_spiSpeedCb = new QComboBox();
  m_spiSpeedCb->addItems({"500000", "1000000", "10000000"});
  ctrlLayout->addWidget(m_spiSpeedCb);

  m_spiSendBtn = new QPushButton("发送/接收(Transfer)");
  connect(m_spiSendBtn, &QPushButton::clicked, this, &BusTestWidget::onSpiSend);

  ctrlLayout->addStretch();
  ctrlLayout->addWidget(m_spiSendBtn);
  layout->addLayout(ctrlLayout);

  QHBoxLayout *splitLayout = new QHBoxLayout();
  QGroupBox *txGroup = new QGroupBox("发送(TX)");
  QVBoxLayout *txBox = new QVBoxLayout(txGroup);
  m_spiTxBox = new QTextEdit();
  m_spiTxBox->setPlaceholderText("在此填入要发送的Hex,例如: 01 02 0A FF");
  txBox->addWidget(m_spiTxBox);

  QGroupBox *rxGroup = new QGroupBox("接收(RX)");
  QVBoxLayout *rxBox = new QVBoxLayout(rxGroup);
  m_spiRxBox = new QTextEdit();
  m_spiRxBox->setReadOnly(true);
  rxBox->addWidget(m_spiRxBox);

  splitLayout->addWidget(txGroup);
  splitLayout->addWidget(rxGroup);
  layout->addLayout(splitLayout);

  EventBus::getInstance()->subscribe(
      "svc/pub/bus/spi_rx", this, [this](const QVariant &payload) {
        QByteArray data = payload.toByteArray();
        m_spiRxBox->append(QString(data.toHex(' ').toUpper()));
      });
}

void BusTestWidget::setupCanTab(QWidget *tab) {
  QVBoxLayout *layout = new QVBoxLayout(tab);
  QHBoxLayout *ctrlLayout = new QHBoxLayout();

  ctrlLayout->addWidget(new QLabel("CAN接口:"));
  m_canDevCb = new QComboBox();
  m_canDevCb->addItems({"can0", "can1"});
  ctrlLayout->addWidget(m_canDevCb);

  ctrlLayout->addWidget(new QLabel("波特率:"));
  m_canBaudCb = new QComboBox();
  m_canBaudCb->addItems({"125000", "250000", "500000", "1000000"});
  ctrlLayout->addWidget(m_canBaudCb);

  m_canUpBtn = new QPushButton("启用(Up)");
  m_canDownBtn = new QPushButton("停用(Down)");
  m_canDownBtn->setEnabled(false);

  connect(m_canUpBtn, &QPushButton::clicked, this, &BusTestWidget::onCanUp);
  connect(m_canDownBtn, &QPushButton::clicked, this, &BusTestWidget::onCanDown);

  ctrlLayout->addStretch();
  ctrlLayout->addWidget(m_canUpBtn);
  ctrlLayout->addWidget(m_canDownBtn);

  layout->addLayout(ctrlLayout);

  QHBoxLayout *splitLayout = new QHBoxLayout();
  QGroupBox *txGroup = new QGroupBox("发送区(TX)");
  QVBoxLayout *txBox = new QVBoxLayout(txGroup);
  m_canTxBox = new QTextEdit();
  m_canTxBox->setPlaceholderText("ID#DATA 例: 123#11223344");
  m_canSendBtn = new QPushButton("发送报文");
  connect(m_canSendBtn, &QPushButton::clicked, this, &BusTestWidget::onCanSend);

  txBox->addWidget(m_canTxBox);
  txBox->addWidget(m_canSendBtn);

  QGroupBox *rxGroup = new QGroupBox("接收区(RX)");
  QVBoxLayout *rxBox = new QVBoxLayout(rxGroup);
  m_canRxBox = new QTextEdit();
  m_canRxBox->setReadOnly(true);
  rxBox->addWidget(m_canRxBox);

  splitLayout->addWidget(txGroup);
  splitLayout->addWidget(rxGroup);
  layout->addLayout(splitLayout);

  EventBus::getInstance()->subscribe("svc/pub/bus/can_rx", this,
                                     [this](const QVariant &payload) {
                                       QString msg = payload.toString();
                                       m_canRxBox->append(msg);
                                     });
}

void BusTestWidget::onI2cRead() {
  QVariantMap req;
  req["dev"] = m_i2cDevCb->currentText();
  req["addr"] = m_i2cAddrCb->currentText().toInt(nullptr, 16);
  req["reg"] = m_i2cRegCb->currentText().toInt(nullptr, 16);
  req["len"] = 1; // 临时测试只读一字节
  EventBus::getInstance()->publish("svc/req/bus/i2c_read", req);
}

void BusTestWidget::onI2cWrite() {
  QVariantMap req;
  req["dev"] = m_i2cDevCb->currentText();
  req["addr"] = m_i2cAddrCb->currentText().toInt(nullptr, 16);
  req["reg"] = m_i2cRegCb->currentText().toInt(nullptr, 16);

  QString txt = m_i2cDataBox->toPlainText();
  QByteArray data = QByteArray::fromHex(txt.toLocal8Bit());
  req["data"] = data;
  EventBus::getInstance()->publish("svc/req/bus/i2c_write", req);
}

void BusTestWidget::onSpiSend() {
  QVariantMap req;
  req["dev"] = m_spiDevCb->currentText();
  req["mode"] = m_spiModeCb->currentIndex();
  req["speed"] = m_spiSpeedCb->currentText().toInt();

  QString txt = m_spiTxBox->toPlainText();
  req["data"] = QByteArray::fromHex(txt.toLocal8Bit());
  EventBus::getInstance()->publish("svc/req/bus/spi_transfer", req);
}

void BusTestWidget::onCanUp() {
  QVariantMap req;
  req["dev"] = m_canDevCb->currentText();
  req["baud"] = m_canBaudCb->currentText().toInt();
  EventBus::getInstance()->publish("svc/req/bus/can_up", req);
  m_canUpBtn->setEnabled(false);
  m_canDownBtn->setEnabled(true);
}

void BusTestWidget::onCanDown() {
  EventBus::getInstance()->publish("svc/req/bus/can_down");
  m_canUpBtn->setEnabled(true);
  m_canDownBtn->setEnabled(false);
}

void BusTestWidget::onCanSend() {
  QString txt = m_canTxBox->toPlainText();
  EventBus::getInstance()->publish("svc/req/bus/can_tx", txt);
}

// --- SysLoadWidget ---
SysLoadWidget::SysLoadWidget(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *l = new QVBoxLayout(this);
  l->addWidget(new QLabel("System Load Info"));
}

void SysLoadWidget::updateSysInfo() {}

// --- LogicAnalyzerView ---
LogicAnalyzerView::LogicAnalyzerView(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *l = new QVBoxLayout(this);
  l->addWidget(new QLabel("Logic Analyzer View"));
}
LogicAnalyzerView::~LogicAnalyzerView() {}
void LogicAnalyzerView::onReadyReadUdp() {}
void LogicAnalyzerView::onItemChanged(QTreeWidgetItem *, int) {}
void LogicAnalyzerView::onBtnActionClicked() {}
void LogicAnalyzerView::onTreeSelectionChanged() {}
void LogicAnalyzerView::onProcessFinished(int, QProcess::ExitStatus) {}

// --- WaveformPlotWidget ---
WaveformPlotWidget::WaveformPlotWidget(QWidget *parent) : QWidget(parent) {}
void WaveformPlotWidget::paintEvent(QPaintEvent *) {}

// --- AudioTestWidget ---
AudioTestWidget::AudioTestWidget(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);

  statusLabel = new QLabel(QString::fromUtf8("就绪，准备录制..."), this);
  statusLabel->setAlignment(Qt::AlignCenter);
  statusLabel->setWordWrap(true);
  layout->addWidget(statusLabel);

  QHBoxLayout *btnLayout = new QHBoxLayout();

  recordBtn = new QPushButton(QString::fromUtf8("🔴 按住说话"), this);
  recordBtn->setMinimumHeight(80);
  btnLayout->addWidget(recordBtn);

  playBtn = new QPushButton(QString::fromUtf8("▶ 播放回放"), this);
  playBtn->setMinimumHeight(80);
  playBtn->setEnabled(false); // Enable after recording
  btnLayout->addWidget(playBtn);

  layout->addLayout(btnLayout);

  connect(recordBtn, &QPushButton::pressed, this,
          &AudioTestWidget::startRecording);
  connect(recordBtn, &QPushButton::released, this,
          &AudioTestWidget::stopRecording);

  connect(playBtn, &QPushButton::clicked, this,
          &AudioTestWidget::onPlayBtnClicked);

  // 监听大模型的 AI 识别结果
  EventBus::getInstance()->subscribe(
      "svc/pub/bus/asr_result", this, [this](const QVariant &payload) {
        QString text = payload.toString();
        if (!text.isEmpty()) {
          statusLabel->setText("AI识别结果: " + text);
          qDebug() << "============ AI 语音识别输出: " << text
                   << " ============";
        }
      });

  // 监听来自底层的播放/录音状态回调
  EventBus::getInstance()->subscribe(
      "svc/pub/bus/audio_status", this, [this](const QVariant &payload) {
        QMap<QString, QVariant> map = payload.toMap();
        QString msg = map["msg"].toString();
        bool isFinished = map["isFinished"].toBool();

        statusLabel->setText(msg);
        if (isFinished) {
          playBtn->setEnabled(true);
          recordBtn->setEnabled(true);
          recordBtn->setStyleSheet("");
          if (msg.contains(QString::fromUtf8("录音完毕"))) {
            statusLabel->setText(
                QString::fromUtf8("正在提取音频交给AI分析..."));
            EventBus::getInstance()->publish("svc/req/bus/asr_recognize_wav",
                                             "/tmp/my_voice.wav");
          }
        }
      });
}

void AudioTestWidget::startRecording() {
  playBtn->setEnabled(false);
  recordBtn->setText(QString::fromUtf8("⏹ 正在录音"));
  recordBtn->setStyleSheet("background-color: red; color: white;");

  // 通过 EventBus 把指令发出去，真正录音在底层 alsa-lib 线程中进行
  EventBus::getInstance()->publish("svc/req/bus/audio_record_start",
                                   "/tmp/my_voice.wav");
}

void AudioTestWidget::stopRecording() {
  recordBtn->setText(QString::fromUtf8("🔴 按住说话"));
  recordBtn->setStyleSheet("");

  // 停止录音
  EventBus::getInstance()->publish("svc/req/bus/audio_record_stop", QVariant());
}

void AudioTestWidget::onPlayBtnClicked() {
  playBtn->setEnabled(false);
  recordBtn->setEnabled(false);

  // 直接通知底层播放录制好的音频
  EventBus::getInstance()->publish("svc/req/bus/audio_play_start",
                                   "/tmp/my_voice.wav");
}
