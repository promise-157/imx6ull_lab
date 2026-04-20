#include "HardwareControlPage.h"
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>

HardwareControlPage::HardwareControlPage(QWidget *parent)
    : IAppModule(parent), m_ledState(false), m_buzzerState(false)
{
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("HardwareControlPage { background-color: #f0f0f5; }");
    this->setObjectName("HardwareControlPage");

    setupUI();

    // 默认开机初始化 (如果权限足的话) 并加载初始环境传感器数据
    m_sensorTimer = new QTimer(this);
    connect(m_sensorTimer, &QTimer::timeout, this, &HardwareControlPage::readSensors);
    
    // 打开定时器开始 500ms 一次轮询传感器
    m_sensorTimer->start(500);
}

HardwareControlPage::~HardwareControlPage() {
    stopService();
}

void HardwareControlPage::stopService() {
    if (m_sensorTimer->isActive()) {
        m_sensorTimer->stop();
    }
}

void HardwareControlPage::writeSysFs(const QString &path, const QString &val) {
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << val;
        file.close();
    } else {
        qDebug() << "Failed to write to sysfs:" << path;
    }
}

QString HardwareControlPage::readSysFs(const QString &path) {
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString data = in.readAll().trimmed();
        file.close();
        return data;
    }
    return QString("");
}

void HardwareControlPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(20);

    // --- 顶部控制栏 ---
    QHBoxLayout *topBar = new QHBoxLayout();
    QLabel *titleLabel = new QLabel(QString::fromUtf8("IMX6ULL 多功能外设测试台"), this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #1e1e24;");
    
    QPushButton *btnHome = new QPushButton(QString::fromUtf8("返回"), this);
    btnHome->setFixedSize(60, 40);
    btnHome->setStyleSheet("QPushButton { background-color: #ff4757; color: white; border-radius: 5px; font-weight: bold; }");
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
    gpioGroup->setStyleSheet("QGroupBox { background: white; border-radius: 8px; border: 1px solid #d1d1e0; font-weight: bold; margin-top: 10px; }"
                             "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 3px 0 3px; }");
    QHBoxLayout *gpioLayout = new QHBoxLayout(gpioGroup);
    
    // LED 按钮
    QVBoxLayout *ledLayout = new QVBoxLayout();
    m_ledBtn = new QPushButton(this);
    m_ledBtn->setFixedSize(80, 80);
    m_ledBtn->setStyleSheet("QPushButton { border-image: url(:/res/images/hwctrl/led_off.svg); background: transparent; }");
    connect(m_ledBtn, &QPushButton::clicked, this, &HardwareControlPage::toggleLed);
    QLabel* ledTip = new QLabel(QString::fromUtf8("用户指示灯 (LED0)"));
    ledTip->setAlignment(Qt::AlignCenter);
    ledLayout->addWidget(m_ledBtn, 0, Qt::AlignCenter);
    ledLayout->addWidget(ledTip);

    // 蜂鸣器 按钮
    QVBoxLayout *buzzerLayout = new QVBoxLayout();
    m_buzzerBtn = new QPushButton(this);
    m_buzzerBtn->setFixedSize(80, 80);
    m_buzzerBtn->setStyleSheet("QPushButton { border-image: url(:/res/images/hwctrl/buzzer_off.svg); background: transparent; }");
    connect(m_buzzerBtn, &QPushButton::clicked, this, &HardwareControlPage::toggleBuzzer);
    QLabel* buzzerTip = new QLabel(QString::fromUtf8("无源蜂鸣器 (BEEP)"));
    buzzerTip->setAlignment(Qt::AlignCenter);
    buzzerLayout->addWidget(m_buzzerBtn, 0, Qt::AlignCenter);
    buzzerLayout->addWidget(buzzerTip);

    gpioLayout->addLayout(ledLayout);
    gpioLayout->addLayout(buzzerLayout);
    gridLayout->addWidget(gpioGroup, 0, 0);

    // ================= 2. PWM 控制区 (屏幕背光) =================
    QGroupBox *pwmGroup = new QGroupBox(QString::fromUtf8("PWM 背光控制"));
    pwmGroup->setStyleSheet("QGroupBox { background: white; border-radius: 8px; border: 1px solid #d1d1e0; font-weight: bold; margin-top: 10px; }"
                            "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 3px 0 3px; }");
    QVBoxLayout *pwmLayout = new QVBoxLayout(pwmGroup);
    
    m_backlightSlider = new QSlider(Qt::Horizontal, this);
    m_backlightSlider->setRange(1, 100); 
    // 读取系统当前亮度作为初值，这里先默认 80。
    // 如果系统真有背光节点，我们把最大值从sysfs拿出来
    m_backlightSlider->setValue(80);
    m_backlightSlider->setStyleSheet("QSlider::groove:horizontal { border: 1px solid #999999; height: 15px; background: #e6e6e6; border-radius: 7px; }"
                                     "QSlider::handle:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f); border: 1px solid #5c5c5c; width: 25px; margin: -5px 0; border-radius: 12px; }"
                                     "QSlider::sub-page:horizontal { background: #1e90ff; border-radius: 7px; }");
    connect(m_backlightSlider, &QSlider::valueChanged, this, &HardwareControlPage::onBacklightChanged);
    
    QLabel* pwmTip = new QLabel(QString::fromUtf8("拖动调节设备 LCD 屏幕亮度  (/sys/class/backlight/)"));
    
    pwmLayout->addStretch();
    pwmLayout->addWidget(m_backlightSlider);
    pwmLayout->addWidget(pwmTip, 0, Qt::AlignCenter);
    pwmLayout->addStretch();

    gridLayout->addWidget(pwmGroup, 0, 1);

    // ================= 3. SPI/I2C 传感器区 (只读数据) =================
    QGroupBox *sensorGroup = new QGroupBox(QString::fromUtf8("I2C/SPI 总线数据 (温湿度/光感/陀螺仪)"));
    sensorGroup->setStyleSheet("QGroupBox { background: white; border-radius: 8px; border: 1px solid #d1d1e0; font-weight: bold; margin-top: 10px; }"
                               "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 3px 0 3px; }");
    QVBoxLayout *sensorLayout = new QVBoxLayout(sensorGroup);
    
    m_ambientLightLabel = new QLabel(QString::fromUtf8("环境光强 (ALS): -- lux"), this);
    m_proximityLabel = new QLabel(QString::fromUtf8("距离感应 (PS): -- mm"), this);
    m_imuDataLabel = new QLabel(QString::fromUtf8("ICM20608 陀螺仪: [x:0.0, y:0.0, z:0.0]"), this);
    
    QString labelStyle = "background-color: #f8f9fa; border-radius: 4px; padding: 10px; font-family: monospace; font-size: 16px;";
    m_ambientLightLabel->setStyleSheet(labelStyle);
    m_proximityLabel->setStyleSheet(labelStyle);
    m_imuDataLabel->setStyleSheet(labelStyle);
    
    sensorLayout->addWidget(m_ambientLightLabel);
    sensorLayout->addWidget(m_proximityLabel);
    sensorLayout->addWidget(m_imuDataLabel);

    gridLayout->addWidget(sensorGroup, 1, 0, 1, 2);

    mainLayout->addLayout(gridLayout, 1);
}

void HardwareControlPage::toggleLed() {
    m_ledState = !m_ledState;
    if (m_ledState) {
        // 设置 UI 和真实硬件节点状态 (绝大数正点原子等引脚分配方式)
        m_ledBtn->setStyleSheet("QPushButton { border-image: url(:/res/images/hwctrl/led_on.svg); background: transparent; }");
        writeSysFs("/sys/class/leds/sys-led/brightness", "1");
    } else {
        m_ledBtn->setStyleSheet("QPushButton { border-image: url(:/res/images/hwctrl/led_off.svg); background: transparent; }");
        writeSysFs("/sys/class/leds/sys-led/brightness", "0");
    }
}

void HardwareControlPage::toggleBuzzer() {
    m_buzzerState = !m_buzzerState;
    if (m_buzzerState) {
        m_buzzerBtn->setStyleSheet("QPushButton { border-image: url(:/res/images/hwctrl/buzzer_on.svg); background: transparent; }");
        // 假设蜂鸣器被映射到 gpio_7 or sysfs
        writeSysFs("/sys/class/gpio/beep/value", "1");
    } else {
        m_buzzerBtn->setStyleSheet("QPushButton { border-image: url(:/res/images/hwctrl/buzzer_off.svg); background: transparent; }");
        writeSysFs("/sys/class/gpio/beep/value", "0");
    }
}

void HardwareControlPage::onBacklightChanged(int value) {
    // value 范围 10-100（避免屏幕黑掉看不见）
    int safeVal = qMax(1, value);
    // 这里假设 maximum_brightness = 100 或者 255。我们在板子上测试可改为适合的值。
    // 这里做基础占位
    writeSysFs("/sys/class/backlight/backlight/brightness", QString::number(safeVal));
}

void HardwareControlPage::readSensors() {
    // 假设板子有 AP3216C 挂在 I2C1 设备名为 ap3216c
    // 真机路径通常类似 /sys/bus/i2c/devices/1-001e/als 或者通过 input subsystem
    // 我们在这里使用一种容错的读取模拟：先尝试读取真机，失败则显示模拟跳动值
    
    QString alsStr = readSysFs("/sys/bus/i2c/devices/1-001e/als");
    if(!alsStr.isEmpty()) {
        m_ambientLightLabel->setText(QString("环境光强 (ALS): %1 lux").arg(alsStr));
    } else {
        // 如果文件不存在（如在虚拟机运行），产生轻量随机干扰展示动态效果
        m_ambientLightLabel->setText(QString("休眠/未接传感器 (ALS): %1 lux模拟").arg(100 + rand() % 50));
        m_proximityLabel->setText(QString::fromUtf8("休眠/未接传感器 (PS): 较远"));
        m_imuDataLabel->setText(QString("陀螺仪震动模拟: [x:%1 , y: %2 , z: 9.8]  (无真实 I2C 总线回传)")
            .arg((rand()%10-5)/10.0).arg((rand()%10-5)/10.0));
    }
}
