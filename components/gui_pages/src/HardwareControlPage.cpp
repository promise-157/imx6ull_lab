#include "HardwareControlPage.h"
#include "EventBus.h"
#include <QDebug>
#include <QFile>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QStyle>
#include <QTextStream>
#include <QVBoxLayout>

HardwareControlPage::HardwareControlPage(QWidget *parent)
    : IAppModule(parent), m_ledState(false), m_buzzerState(false) {
  this->setAttribute(Qt::WA_StyledBackground, true);
  this->setObjectName("HardwareControlPage");

  QFile file(":/res/style/HardwareControlPage.qss");
  if (file.open(QFile::ReadOnly)) {
    this->setStyleSheet(QLatin1String(file.readAll()));
    file.close();
  }

  setupUI();

  EventBus::getInstance()->subscribe(
      "svc/pub/hw/led_state", this, [this](const QVariant &payload) {
        m_ledState = payload.toBool();
        m_ledBtn->setProperty("isOn", m_ledState);
        style()->unpolish(m_ledBtn);
        style()->polish(m_ledBtn);
      });

  EventBus::getInstance()->subscribe(
      "svc/pub/hw/buzzer_state", this, [this](const QVariant &payload) {
        m_buzzerState = payload.toBool();
        m_buzzerBtn->setProperty("isOn", m_buzzerState);
        style()->unpolish(m_buzzerBtn);
        style()->polish(m_buzzerBtn);
      });

  EventBus::getInstance()->subscribe(
      "svc/pub/hw/sensor/als", this, [this](const QVariant &payload) {
        m_ambientLightLabel->setText(
            QString("环境光强 (ALS): %1 lux").arg(payload.toString()));
      });

  EventBus::getInstance()->subscribe(
      "svc/pub/hw/sensor/ps", this, [this](const QVariant &payload) {
        m_proximityLabel->setText(
            QString("距离感应 (PS): %1").arg(payload.toString()));
      });

  EventBus::getInstance()->subscribe(
      "svc/pub/hw/sensor/imu", this, [this](const QVariant &payload) {
        m_imuDataLabel->setText(
            QString("ICM20608 陀螺仪: %1").arg(payload.toString()));
      });
}

HardwareControlPage::~HardwareControlPage() { stopService(); }

void HardwareControlPage::stopService() {
  // UI 页面停止时无需直接管理底层硬件轮询线程
}

void HardwareControlPage::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(15, 15, 15, 15);
  mainLayout->setSpacing(20);

  // --- 顶部控制栏 ---
  QHBoxLayout *topBar = new QHBoxLayout();
  QLabel *titleLabel =
      new QLabel(QString::fromUtf8("IMX6ULL 多功能外设测试台"), this);
  titleLabel->setObjectName("HwTitleLabel");

  QPushButton *btnHome = new QPushButton(QString::fromUtf8("返回"), this);
  btnHome->setFixedSize(60, 40);
  btnHome->setObjectName("HwBtnHome");
  connect(btnHome, &QPushButton::clicked, this, &IAppModule::requestClose);

  topBar->addWidget(titleLabel);
  topBar->addStretch();
  topBar->addWidget(btnHome);

  mainLayout->addLayout(topBar);

  // --- 硬件区网格布局 ---
  QGridLayout *gridLayout = new QGridLayout();
  gridLayout->setSpacing(15);

  // ================= 1. GPIO 控制区 (LED / 蜂鸣器) =================
  QGroupBox *gpioGroup = new QGroupBox(QString::fromUtf8("外部数字 IO"));
  gpioGroup->setObjectName("HwGroupBox");
  QHBoxLayout *gpioLayout = new QHBoxLayout(gpioGroup);

  // LED 按钮
  QVBoxLayout *ledLayout = new QVBoxLayout();
  m_ledBtn = new QPushButton(this);
  m_ledBtn->setFixedSize(80, 80);
  m_ledBtn->setObjectName("HwLedBtn");
  // 不再使用setStyleSheet，改用动态属性来驱动QSS图片切换
  m_ledBtn->setProperty("isOn", false);
  connect(m_ledBtn, &QPushButton::clicked, this,
          &HardwareControlPage::toggleLed);
  QLabel *ledTip = new QLabel(QString::fromUtf8("用户指示灯 (LED0)"));
  ledTip->setAlignment(Qt::AlignCenter);
  ledLayout->addWidget(m_ledBtn, 0, Qt::AlignCenter);
  ledLayout->addWidget(ledTip);

  // 蜂鸣器 按钮
  QVBoxLayout *buzzerLayout = new QVBoxLayout();
  m_buzzerBtn = new QPushButton(this);
  m_buzzerBtn->setFixedSize(80, 80);
  m_buzzerBtn->setObjectName("HwBuzzerBtn");
  m_buzzerBtn->setProperty("isOn", false);
  connect(m_buzzerBtn, &QPushButton::clicked, this,
          &HardwareControlPage::toggleBuzzer);
  QLabel *buzzerTip = new QLabel(QString::fromUtf8("无源蜂鸣器 (BEEP)"));
  buzzerTip->setAlignment(Qt::AlignCenter);
  buzzerLayout->addWidget(m_buzzerBtn, 0, Qt::AlignCenter);
  buzzerLayout->addWidget(buzzerTip);

  gpioLayout->addLayout(ledLayout);
  gpioLayout->addLayout(buzzerLayout);
  gridLayout->addWidget(gpioGroup, 0, 0);

  // ================= 2. PWM 控制区 (屏幕背光) =================
  QGroupBox *pwmGroup = new QGroupBox(QString::fromUtf8("PWM 背光控制"));
  pwmGroup->setObjectName("HwGroupBox");
  QVBoxLayout *pwmLayout = new QVBoxLayout(pwmGroup);

  m_backlightSlider = new QSlider(Qt::Horizontal, this);
  m_backlightSlider->setRange(1, 100);
  m_backlightSlider->setObjectName("HwBacklightSlider");
  // 读取系统当前亮度作为初值，这里先默认 80。
  // 如果系统真有背光节点，我们把最大值从sysfs拿出来
  m_backlightSlider->setValue(80);
  connect(m_backlightSlider, &QSlider::valueChanged, this,
          &HardwareControlPage::onBacklightChanged);

  QLabel *pwmTip = new QLabel(
      QString::fromUtf8("拖动调节设备 LCD 屏幕亮度  (/sys/class/backlight/)"));

  pwmLayout->addStretch();
  pwmLayout->addWidget(m_backlightSlider);
  pwmLayout->addWidget(pwmTip, 0, Qt::AlignCenter);
  pwmLayout->addStretch();

  gridLayout->addWidget(pwmGroup, 0, 1);

  // ================= 3. SPI/I2C 传感器区 (只读数据) =================
  QGroupBox *sensorGroup =
      new QGroupBox(QString::fromUtf8("I2C/SPI 总线数据 (温湿度/光感/陀螺仪)"));
  sensorGroup->setObjectName("HwGroupBox");
  QVBoxLayout *sensorLayout = new QVBoxLayout(sensorGroup);

  m_ambientLightLabel =
      new QLabel(QString::fromUtf8("环境光强 (ALS): -- lux"), this);
  m_proximityLabel =
      new QLabel(QString::fromUtf8("距离感应 (PS): -- mm"), this);
  m_imuDataLabel = new QLabel(
      QString::fromUtf8("ICM20608 陀螺仪: [x:0.0, y:0.0, z:0.0]"), this);

  m_ambientLightLabel->setObjectName("HwSensorLabel");
  m_proximityLabel->setObjectName("HwSensorLabel");
  m_imuDataLabel->setObjectName("HwSensorLabel");

  sensorLayout->addWidget(m_ambientLightLabel);
  sensorLayout->addWidget(m_proximityLabel);
  sensorLayout->addWidget(m_imuDataLabel);

  gridLayout->addWidget(sensorGroup, 1, 0, 1, 2);

  mainLayout->addLayout(gridLayout, 1);
}

void HardwareControlPage::toggleLed() {
  EventBus::getInstance()->publish("svc/req/hw/led_toggle");
}

void HardwareControlPage::toggleBuzzer() {
  EventBus::getInstance()->publish("svc/req/hw/buzzer_toggle");
}

void HardwareControlPage::onBacklightChanged(int value) {
  EventBus::getInstance()->publish("svc/req/hw/backlight_set",
                                   QVariant::fromValue(value));
}

void HardwareControlPage::readSensors() {
  // 传感器读取已下沉到 HardwareService
}
