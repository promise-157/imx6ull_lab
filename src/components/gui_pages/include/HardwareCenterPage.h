#ifndef HARDWARE_CENTER_PAGE_H
#define HARDWARE_CENTER_PAGE_H

#include "IAppModule.h"
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QPaintEvent>
#include <QProcess>
#include <QPushButton>
#include <QSlider>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTreeWidget>

// Widget prototypes
class BasicIoWidget : public QWidget {
  Q_OBJECT
public:
  explicit BasicIoWidget(QWidget *parent = nullptr);
};

class BusTestWidget : public QWidget {
  Q_OBJECT
public:
  explicit BusTestWidget(QWidget *parent = nullptr);

private:
  void setupUI();
  void setupUartTab(QWidget *tab);
  void setupI2cTab(QWidget *tab);
  void setupSpiTab(QWidget *tab);
  void setupCanTab(QWidget *tab);

  // UART
  QComboBox *m_uartBaudCb;
  QPushButton *m_uartOpenBtn;
  QPushButton *m_uartCloseBtn;
  QTextEdit *m_uartRxEdit;
  QTextEdit *m_uartTxEdit;
  QPushButton *m_uartSendBtn;

  // I2C
  QComboBox *m_i2cDevCb;
  QComboBox *m_i2cAddrCb;
  QComboBox *m_i2cRegCb;
  QPushButton *m_i2cReadBtn;
  QPushButton *m_i2cWriteBtn;
  QTextEdit *m_i2cDataBox;

  // SPI
  QComboBox *m_spiDevCb;
  QComboBox *m_spiModeCb;
  QComboBox *m_spiSpeedCb;
  QPushButton *m_spiSendBtn;
  QTextEdit *m_spiTxBox;
  QTextEdit *m_spiRxBox;

  // CAN
  QComboBox *m_canDevCb;
  QComboBox *m_canBaudCb;
  QPushButton *m_canUpBtn;
  QPushButton *m_canDownBtn;
  QPushButton *m_canSendBtn;
  QTextEdit *m_canTxBox;
  QTextEdit *m_canRxBox;

private slots:
  void onUartOpen();
  void onUartClose();
  void onUartSend();

  void onI2cRead();
  void onI2cWrite();

  void onSpiSend();

  void onCanUp();
  void onCanDown();
  void onCanSend();
};

class SysLoadWidget : public QWidget {
  Q_OBJECT
public:
  explicit SysLoadWidget(QWidget *parent = nullptr);
public slots:
  void updateSysInfo();
};

class AudioTestWidget : public QWidget {
  Q_OBJECT
public:
  explicit AudioTestWidget(QWidget *parent = nullptr);

private slots:
  void startRecording();
  void stopRecording();
  void onPlayBtnClicked();

private:
  QPushButton *recordBtn;
  QPushButton *playBtn;
  QLabel *statusLabel;
};

class WaveformPlotWidget : public QWidget {
  Q_OBJECT
public:
  explicit WaveformPlotWidget(QWidget *parent = nullptr);

protected:
  void paintEvent(QPaintEvent *event) override;
};

class LogicAnalyzerView : public QWidget {
  Q_OBJECT
public:
  explicit LogicAnalyzerView(QWidget *parent = nullptr);
  ~LogicAnalyzerView() override;
public slots:
  void onReadyReadUdp();
  void onItemChanged(QTreeWidgetItem *item, int column);
  void onBtnActionClicked();
  void onTreeSelectionChanged();
  void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

class HardwareCenterPage : public IAppModule {
  Q_OBJECT
public:
  explicit HardwareCenterPage(QWidget *parent = nullptr);
  ~HardwareCenterPage() override;

  // IAppModule implementation
  QString appName() const override { return QString::fromUtf8("外设诊断台"); }
  QString appIconPath() const override {
    return ":/res/images/app_icons/hardware.png";
  }

  void stopService();

private:
  void setupUI();

  QTreeWidget *m_menuTree;
  QStackedWidget *m_contentStack;
};

#endif // HARDWARE_CENTER_PAGE_H
