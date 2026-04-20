#ifndef HARDWAREMONITORPAGE_H
#define HARDWAREMONITORPAGE_H

#include "IAppModule.h"
#include <QUdpSocket>
#include <QWidget>
#include <QVector>
#include <QMap>
#include <QTimer>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QScrollArea>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QProcess>
#include <QPushButton>

// ==========================
// 1. 逻辑波形绘制核心组件
// ==========================
class WaveformPlotWidget : public QWidget {
    Q_OBJECT
public:
    explicit WaveformPlotWidget(QWidget* parent = nullptr);
    
    struct Sample {
        qint64 timestamp;
        int value;
    };
    
    void pushSample(const QString& channel, int value);
    void setChannelVisible(const QString& channel, bool visible);
    
protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QMap<QString, QVector<Sample>> m_channelData;
    QMap<QString, bool> m_channelVisible;
    qint64 m_startTime;
    
    friend class LogicAnalyzerView;
};

// ==========================
// 1.5 逻辑分析仪主视图 (左侧进程和资源树 + 右侧波形)
// ==========================
class LogicAnalyzerView : public QWidget {
    Q_OBJECT
public:
    explicit LogicAnalyzerView(QWidget* parent = nullptr);
    ~LogicAnalyzerView();
    void startMonitor();
    void stopMonitor();

private slots:
    void onReadyReadUdp();
    void onItemChanged(QTreeWidgetItem* item, int column);
    
    // 应用启停测控制
    void onBtnActionClicked();
    void onTreeSelectionChanged();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void initResourceTree();
    void loadRunningProcesses();
    void updateChannelFromProbe(const QString& channel);

    WaveformPlotWidget* m_plotWidget;
    QScrollArea* m_scrollArea;
    QTreeWidget* m_resourceTree;
    QTimer* m_timer;
    QUdpSocket* m_udpSocket;

    // 选中的测试程序目标与状态管理
    QProcess* m_probeProcess; 
    QPushButton* m_refreshBtn;
    QPushButton* m_actionBtn;
    QLabel* m_appDescLabel;
    
    QTreeWidgetItem* m_appRoot;
    QTreeWidgetItem* m_hardwareRoot;
    
    // 映射表：pid -> tree item
    QMap<int, QTreeWidgetItem*> m_procItems;
};

// ==========================
// 2. 系统负载面板
// ==========================
class SysLoadWidget : public QWidget {
    Q_OBJECT
public:
    explicit SysLoadWidget(QWidget* parent = nullptr);

private slots:
    void updateSysInfo();

private:
    QProgressBar* m_cpuBar;
    QProgressBar* m_memBar;
    QLabel* m_detailsLabel;
    QTimer* m_timer;
    
    long long m_lastTotalUser, m_lastTotalUserLow, m_lastTotalSys, m_lastTotalIdle;
};

// ==========================
// 主应用界面
// ==========================
class HardwareMonitorPage : public IAppModule {
    Q_OBJECT
public:
    explicit HardwareMonitorPage(QWidget *parent = nullptr);
    ~HardwareMonitorPage() override;

    QString appName() const override { return QString::fromUtf8("硬件监控中心"); }
    QString appIconPath() const override { return QString::fromUtf8(":/res/images/hardware/monitor_icon.png"); }
    void stopService() override;

private:
    void setupUI();

    QListWidget* m_sideMenu;
    QStackedWidget* m_stackWidget;
    
    LogicAnalyzerView* m_logicAnalyzer;
    SysLoadWidget* m_sysLoadInfo;
};

#endif // HARDWAREMONITORPAGE_H