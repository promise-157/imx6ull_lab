#ifndef HARDWARECONTROLPAGE_H
#define HARDWARECONTROLPAGE_H

#include "IAppModule.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>

class HardwareControlPage : public IAppModule {
    Q_OBJECT
public:
    explicit HardwareControlPage(QWidget *parent = nullptr);
    ~HardwareControlPage() override;

    QString appName() const override { return QString::fromUtf8("硬件外设控制"); }
    // 如果之后有统一大图标可以替换，先借用硬件监控的图标，或自带字符串
    QString appIconPath() const override { return QString::fromUtf8(":/res/images/app_icons/hardware.png"); }
    
    void stopService() override;

private slots:
    void toggleLed();
    void toggleBuzzer();
    void onBacklightChanged(int value);
    void readSensors();

private:
    void setupUI();

    bool m_ledState;
    bool m_buzzerState;

    QPushButton* m_ledBtn;
    QPushButton* m_buzzerBtn;
    QSlider* m_backlightSlider;
    QLabel* m_ambientLightLabel;
    QLabel* m_proximityLabel;
    QLabel* m_imuDataLabel;
};

#endif // HardwareControlPage_H
