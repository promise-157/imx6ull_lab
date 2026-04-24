#include "HardwareMonitorPage.h"
#include "EventBus.h"
#include <QPainter>
#include <QDateTime>
#include <QPen>
#include <QBrush>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QHeaderView>
#include <QDebug>

// ==========================================
// 1. 核心波形绘制组件 (绘图区)
// ==========================================
WaveformPlotWidget::WaveformPlotWidget(QWidget* parent) : QWidget(parent) {
    m_startTime = QDateTime::currentMSecsSinceEpoch();
    setMinimumHeight(400); 
}

void WaveformPlotWidget::pushSample(const QString& channel, int value) {
    Sample s;
    s.timestamp = QDateTime::currentMSecsSinceEpoch() - m_startTime;
    s.value = value;
    m_channelData[channel].append(s);
    
    const qint64 WIN_SIZE = 10000;
    while (!m_channelData[channel].isEmpty() && m_channelData[channel].first().timestamp < (s.timestamp - WIN_SIZE)) {
        m_channelData[channel].removeFirst();
    }
}

void WaveformPlotWidget::setChannelVisible(const QString& channel, bool visible) {
    m_channelVisible[channel] = visible;
    update();
}

void WaveformPlotWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor(25, 25, 25)); 
    
    QStringList visibleChannels;
    for (auto it = m_channelData.constBegin(); it != m_channelData.constEnd(); ++it) {
        if(m_channelVisible.value(it.key(), true)) visibleChannels.append(it.key());
    }
    
    for(auto it = m_channelVisible.constBegin(); it != m_channelVisible.constEnd(); ++it) {
        if(it.value() && !visibleChannels.contains(it.key())) visibleChannels.append(it.key());
    }
    
    if (visibleChannels.isEmpty()) {
        p.setPen(Qt::white);
        p.drawText(rect(), Qt::AlignCenter, QString::fromUtf8("未选择任何硬件/变量资源。\n请在左侧菜单展开选择需要监控的目标..."));
        return;
    }
    
    int rowHeight = 80;
    int requiredHeight = std::max(height(), visibleChannels.size() * rowHeight);
    if(requiredHeight != minimumHeight()) {
        setMinimumHeight(requiredHeight);
    }
    rowHeight = std::max(60, requiredHeight / visibleChannels.size());
    
    qint64 currentT = QDateTime::currentMSecsSinceEpoch() - m_startTime;
    qint64 tMin = currentT - 10000;
    int channelIdx = 0;
    
    for (const QString& name : visibleChannels) {
        const QVector<Sample>& samples = m_channelData.value(name);
        
        QRect channelRect(0, channelIdx * rowHeight, width(), rowHeight);
        p.setPen(QPen(Qt::darkGray, 1, Qt::DashLine));
        p.drawLine(channelRect.bottomLeft(), channelRect.bottomRight());
        
        p.setPen(QColor(100, 200, 255));
        p.drawText(channelRect.topLeft() + QPoint(10, 20), name);
        
        p.setPen(QPen(QColor(0, 255, 100), 2));
        if (!samples.isEmpty()) {
            QPointF lastPoint;
            bool firstPoint = true;
            for (int i=0; i<samples.size(); ++i) {
                const Sample& s = samples[i];
                float normalizedVal = (s.value == 0) ? 0.0f : 1.0f; 
                int px = ((s.timestamp - tMin) * width()) / 10000.0f;
                int py = channelRect.bottom() - 10 - (normalizedVal * (rowHeight - 40));
                
                if (firstPoint) {
                    p.drawLine(QPointF(0, py), QPointF(px, py));
                    lastPoint = QPointF(px, py);
                    firstPoint = false;
                } else {
                    QPointF stepPoint(px, lastPoint.y());
                    p.drawLine(lastPoint, stepPoint); 
                    p.drawLine(stepPoint, QPointF(px, py));
                    lastPoint = QPointF(px, py);
                }
            }
            p.drawLine(lastPoint, QPointF(width(), lastPoint.y()));
        } else {
            p.setPen(QPen(QColor(80, 80, 80), 2));
            int py = channelRect.bottom() - 10;
            p.drawLine(QPointF(0, py), QPointF(width(), py));
        }
        channelIdx++;
    }
}

// ==========================================
// 1.5 逻辑分析仪主视图 (左侧多级进程树 + 右侧波形)
// ==========================================
LogicAnalyzerView::LogicAnalyzerView(QWidget* parent) : QWidget(parent), m_plotWidget(nullptr), m_udpSocket(nullptr) {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    
    // ==== 左侧资源面板 ====
    QWidget* listPanel = new QWidget(this);
    listPanel->setFixedWidth(260); // 必须定宽
    listPanel->setStyleSheet("background-color: #f9f9f9; border-right: 1px solid #ccc;");
    QVBoxLayout* listLayout = new QVBoxLayout(listPanel);
    listLayout->setContentsMargins(5, 5, 5, 5);
    
    // 标题与刷新区
    QWidget* titleWidget = new QWidget(listPanel);
    QHBoxLayout* titleHLayout = new QHBoxLayout(titleWidget);
    titleHLayout->setContentsMargins(0,0,0,0);
    QLabel* tipLabel = new QLabel(QString::fromUtf8("可监视底层接口/进程变量"), titleWidget);
    tipLabel->setStyleSheet("color: #444; font-size: 12px; font-weight: bold; border: none;");
    m_refreshBtn = new QPushButton(QString::fromUtf8("⟳"), titleWidget);
    m_refreshBtn->setFixedSize(30, 24);
    m_refreshBtn->setToolTip(QString::fromUtf8("刷新运行中的进程"));
    titleHLayout->addWidget(tipLabel, 1);
    titleHLayout->addWidget(m_refreshBtn);
    listLayout->addWidget(titleWidget);
    
    // 初始化多级树形菜单
    m_resourceTree = new QTreeWidget(listPanel);
    m_resourceTree->setHeaderHidden(true);
    m_resourceTree->setStyleSheet("QTreeWidget { background: transparent; border: none; outline: none; } "
                                  "QTreeWidget::item { padding: 4px; border-bottom: 1px solid #eaeaea; } "
                                  "QTreeWidget::item:selected { background-color: #d0e4ff; color: #000; }");
    listLayout->addWidget(m_resourceTree, 1);
    
    // 重点操作：动态控制区面板
    QWidget* appControlPanel = new QWidget(listPanel);
    appControlPanel->setStyleSheet("background-color: #eee; border-radius: 4px;");
    QVBoxLayout* appCtrlLayout = new QVBoxLayout(appControlPanel);
    appCtrlLayout->setContentsMargins(5, 5, 5, 5);
    
    m_appDescLabel = new QLabel(QString::fromUtf8("未选择程序"), appControlPanel);
    m_appDescLabel->setStyleSheet("color: #555; font-size: 11px;");
    m_appDescLabel->setWordWrap(true);
    
    m_actionBtn = new QPushButton(QString::fromUtf8("请选中可挂载程序"), appControlPanel);
    m_actionBtn->setEnabled(false);
    m_actionBtn->setStyleSheet("QPushButton { background-color: #aaa; color: white; border-radius: 4px; padding: 6px; font-weight: bold; } "
                               "QPushButton:enabled { background-color: #0055ff; } QPushButton:enabled:hover { background-color: #0044cc; }");
    
    appCtrlLayout->addWidget(m_appDescLabel);
    appCtrlLayout->addWidget(m_actionBtn);
    listLayout->addWidget(appControlPanel);
    
    // 初始化网络探针与事件
    m_probeProcess = new QProcess(this);
    connect(m_probeProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), 
            this, &LogicAnalyzerView::onProcessFinished);
    connect(m_actionBtn, &QPushButton::clicked, this, &LogicAnalyzerView::onBtnActionClicked);
    connect(m_refreshBtn, &QPushButton::clicked, this, &LogicAnalyzerView::loadRunningProcesses);
    
    m_plotWidget = new WaveformPlotWidget(this);
    
    initResourceTree();
    connect(m_resourceTree, &QTreeWidget::itemChanged, this, &LogicAnalyzerView::onItemChanged);
    connect(m_resourceTree, &QTreeWidget::itemSelectionChanged, this, &LogicAnalyzerView::onTreeSelectionChanged);

    // 直接板级数据链路：通过 Service/HAL 事件输入，不依赖 UDP 脚本。
    EventBus::getInstance()->subscribe(
        "svc/pub/hw/led_state", this, [this](const QVariant& payload) {
            if (m_plotWidget) {
                m_plotWidget->pushSample("GPIO1_IO03 (sys-led)", payload.toBool() ? 1 : 0);
            }
        });
    EventBus::getInstance()->subscribe(
        "svc/pub/hw/buzzer_state", this, [this](const QVariant& payload) {
            if (m_plotWidget) {
                m_plotWidget->pushSample("GPIO5_IO01 (beep)", payload.toBool() ? 1 : 0);
            }
        });
    EventBus::getInstance()->subscribe(
        "svc/pub/hw/sensor/als", this, [this](const QVariant& payload) {
            bool ok = false;
            int lux = payload.toString().toInt(&ok);
            int level = (ok && lux >= 120) ? 1 : 0;
            if (m_plotWidget) {
                m_plotWidget->pushSample("AP3216C_ALS", level);
            }
        });
    EventBus::getInstance()->subscribe(
        "svc/pub/hw/sensor/ps", this, [this](const QVariant& payload) {
            const QString ps = payload.toString();
            int level = (ps.contains(QString::fromUtf8("近")) || ps.contains("near")) ? 1 : 0;
            if (m_plotWidget) {
                m_plotWidget->pushSample("AP3216C_PS", level);
            }
        });
    
    // ==== 右侧面板 ====
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidget(m_plotWidget);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setStyleSheet("QScrollArea { border: none; background-color: #191919; }");
    
    mainLayout->addWidget(listPanel);
    mainLayout->addWidget(m_scrollArea, 1);
    
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, m_plotWidget, [this](){ m_plotWidget->update(); });
    
    loadRunningProcesses(); // 初始化时自动扫描一次
}

LogicAnalyzerView::~LogicAnalyzerView() {
    if (m_probeProcess->state() != QProcess::NotRunning) {
        m_probeProcess->kill();
        m_probeProcess->waitForFinished();
    }
}

void LogicAnalyzerView::initResourceTree() {
    m_resourceTree->clear();
    m_procItems.clear();

    m_hardwareRoot = new QTreeWidgetItem(m_resourceTree, QStringList(QString::fromUtf8("IMX6 板载硬件接口")));
    m_hardwareRoot->setFlags(m_hardwareRoot->flags() & ~Qt::ItemIsUserCheckable); 
    m_hardwareRoot->setForeground(0, QBrush(QColor("#cc5500")));
    
    QStringList imx6HWList = {
        "GPIO1_IO03 (sys-led)", "GPIO5_IO01 (beep)", "AP3216C_ALS",
        "AP3216C_PS"};
    for(const QString& hw : imx6HWList) {
        QTreeWidgetItem* item = new QTreeWidgetItem(m_hardwareRoot, QStringList(hw));
        item->setCheckState(0, Qt::Unchecked);
        if (m_plotWidget) {
            m_plotWidget->setChannelVisible(hw, false);
        }
    }
    
    m_appRoot = new QTreeWidgetItem(m_resourceTree, QStringList(QString::fromUtf8("本机进程探针注入 (运行中)")));
    m_appRoot->setFlags(m_appRoot->flags() & ~Qt::ItemIsUserCheckable);
    m_appRoot->setForeground(0, QBrush(QColor("#0055ff")));
    
    // 强制静态测试项，为了演示无论如何可以被启动
    m_scriptRoot = new QTreeWidgetItem(m_resourceTree, QStringList(QString::fromUtf8("★ 本地挂载脚本 (手动启动)")));
    m_scriptRoot->setFlags(m_scriptRoot->flags() & ~Qt::ItemIsUserCheckable);
    m_scriptRoot->setForeground(0, QBrush(QColor("#cc00cc")));
    m_scriptRoot->setData(0, Qt::UserRole, "test_hw_probe.sh"); // 自定义特殊标记数据
    
    // 添加默认子变量
    QTreeWidgetItem* var1 = new QTreeWidgetItem(m_scriptRoot, QStringList("GPIO1_IO3"));
    var1->setCheckState(0, Qt::Unchecked);
    if (m_plotWidget) {
        m_plotWidget->setChannelVisible("GPIO1_IO3", false);
    }
    QTreeWidgetItem* var2 = new QTreeWidgetItem(m_scriptRoot, QStringList("SPI_CLK"));
    var2->setCheckState(0, Qt::Unchecked);
    if (m_plotWidget) {
        m_plotWidget->setChannelVisible("SPI_CLK", false);
    }
    
    m_resourceTree->expandAll();
}

void LogicAnalyzerView::loadRunningProcesses() {
    // 除了基础项，我们将动态读取 /proc 把允许被监听的测试进程显示出来
    int count = m_appRoot->childCount();
    for (int i = count - 1; i >= 0; --i) {
         QTreeWidgetItem* item = m_appRoot->takeChild(i);
         delete item;
    }
    m_procItems.clear();
    
    QDir procDir("/proc");
    QStringList dirs = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    int added = 0;
    foreach(QString dirName, dirs) {
        bool ok;
        int pid = dirName.toInt(&ok);
        if(ok && pid > 500) { // 过滤掉系统极早期守护进程，保留用户的
            QFile cmdFile(QString("/proc/%1/cmdline").arg(dirName));
            if(cmdFile.open(QIODevice::ReadOnly)) {
                QByteArray cmd = cmdFile.readAll();
                if(cmd.length() > 0) {
                    QString cmdStr = QString::fromUtf8(cmd).replace('\0', ' ').trimmed();
                    // 这里我们过滤掉明显无关的进程，比如 bash, sh 或我们自己的主程序
                    if(cmdStr.contains("test_") || cmdStr.contains("app_") || cmdStr.contains("probe") || (cmdStr.endsWith(".elf"))) {
                        // 这是一个适合监听的外部程序
                        QString display = QString("[%1] %2").arg(pid).arg(cmdStr.split(" ").first());
                        QTreeWidgetItem* pItem = new QTreeWidgetItem(m_appRoot, QStringList(display));
                        pItem->setFlags(pItem->flags() & ~Qt::ItemIsUserCheckable); // 进程本身不能被勾选，只能挂载
                        pItem->setData(0, Qt::UserRole, QString::number(pid));
                        
                        // 挂载一个虚拟监听所有变量的占位符 (等待其发来再展开)
                        QTreeWidgetItem* anyVar = new QTreeWidgetItem(pItem, QStringList("等待进程发来变量..."));
                        anyVar->setFlags(anyVar->flags() & ~Qt::ItemIsUserCheckable); // 不让点
                        anyVar->setForeground(0, QBrush(Qt::gray));
                        
                        m_procItems[pid] = pItem;
                        added++;
                    }
                }
                cmdFile.close();
            }
        }
    }
    if(added == 0) {
        QTreeWidgetItem* empty = new QTreeWidgetItem(m_appRoot, QStringList(QString::fromUtf8("(暂无符合条件的进程)")));
        empty->setForeground(0, QBrush(Qt::gray));
        empty->setFlags(Qt::NoItemFlags);
    }
    m_appRoot->setExpanded(true);
}

void LogicAnalyzerView::onTreeSelectionChanged() {
    QList<QTreeWidgetItem*> selected = m_resourceTree->selectedItems();
    if(selected.isEmpty()) return;
    
    QTreeWidgetItem* cur = selected.first();
    // 判断它是不是主应用程序节点
    QString pidData = cur->data(0, Qt::UserRole).toString();
    
    if(pidData == "test_hw_probe.sh") {
        if(m_probeProcess->state() == QProcess::Running) {
            m_appDescLabel->setText(QString::fromUtf8("目标：test_hw_probe.sh\n状态：运行中...\n可勾选下方变量开始监控。"));
            m_actionBtn->setText(QString::fromUtf8("■ 停止测试程序"));
            m_actionBtn->setStyleSheet("background-color: #e63946; color: white; border-radius:4px; font-weight:bold;");
        } else {
            m_appDescLabel->setText(QString::fromUtf8("目标：test_hw_probe.sh\n状态：未运行。\n这是一个全自动生成的测试发生器程序。"));
            m_actionBtn->setText(QString::fromUtf8("▶ 启动程序探针"));
            m_actionBtn->setStyleSheet("background-color: #0055ff; color: white; border-radius:4px; font-weight:bold;");
        }
        m_actionBtn->setEnabled(true);
    } 
    else if(!pidData.isEmpty() && m_procItems.contains(pidData.toInt())) {
        m_appDescLabel->setText(QString::fromUtf8("目标 PID: %1\n状态：正在系统后台运行。\n如果要监听它，请确保它使用LogicClient.h往9090发送UDP数据。").arg(pidData));
        m_actionBtn->setText(QString::fromUtf8("监听进行中 (被动)"));
        m_actionBtn->setEnabled(false);
        m_actionBtn->setStyleSheet("background-color: #2b9348; color: white; border-radius:4px; font-weight:bold;");
    } else {
        m_appDescLabel->setText(QString::fromUtf8("请选中测试探针程序或父级进程进行控制"));
        m_actionBtn->setText(QString::fromUtf8("请选中可挂载程序"));
        m_actionBtn->setEnabled(false);
        m_actionBtn->setStyleSheet("background-color: #aaa; color: white; border-radius:4px; font-weight:bold;");
    }
}

void LogicAnalyzerView::onBtnActionClicked() {
    // 只有特殊本地探针可以直接起停控制
    if(m_probeProcess->state() == QProcess::Running) {
        m_probeProcess->kill();
    } else {
        m_probeProcess->start("./test_hw_probe.sh");
    }
    // 0.2秒后刷新一下按钮UI
    QTimer::singleShot(200, this, &LogicAnalyzerView::onTreeSelectionChanged);
}

void LogicAnalyzerView::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode); Q_UNUSED(exitStatus);
    onTreeSelectionChanged(); // 去更新停止后的按钮文字
}

void LogicAnalyzerView::onItemChanged(QTreeWidgetItem* item, int column) {
    if(!item || !m_plotWidget) return;
    if((item->flags() & Qt::ItemIsUserCheckable) && column == 0) {
        m_plotWidget->setChannelVisible(item->text(0), item->checkState(0) == Qt::Checked);
    }
}

void LogicAnalyzerView::updateChannelFromProbe(const QString& channel, int ownerPid) {
    QTreeWidgetItem* parentItem = nullptr;
    if (ownerPid > 0 && m_procItems.contains(ownerPid)) {
        parentItem = m_procItems.value(ownerPid);
    } else {
        parentItem = m_scriptRoot;
    }
    if (!parentItem) return;

    // 清理进程节点下的占位提示
    if (parentItem != m_scriptRoot && parentItem->childCount() == 1 &&
        parentItem->child(0)->text(0).contains(QString::fromUtf8("等待进程发来变量"))) {
        QTreeWidgetItem* placeholder = parentItem->takeChild(0);
        delete placeholder;
    }

    for(int j = 0; j < parentItem->childCount(); ++j) {
        if(parentItem->child(j)->text(0) == channel) return;
    }

    QTreeWidgetItem* newItem = new QTreeWidgetItem(parentItem, QStringList(channel));
    newItem->setCheckState(0, Qt::Checked); // 新收到的自动打开监控并在界面打勾
    m_plotWidget->setChannelVisible(channel, true);
}

void LogicAnalyzerView::startMonitor() {
    // UDP 仅作为外部探针扩展输入；板级监测主链路来自 EventBus(svc/pub/hw/*)。
    if (!m_udpSocket) {
        m_udpSocket = new QUdpSocket(this);
        m_udpBindOk = m_udpSocket->bind(QHostAddress::Any, 9090);
        if(m_udpBindOk) {
            connect(m_udpSocket, &QUdpSocket::readyRead, this, &LogicAnalyzerView::onReadyReadUdp);
        } else {
            qWarning() << "[LogicAnalyzer] Failed to bind UDP 9090:" << m_udpSocket->errorString();
            m_appDescLabel->setText(QString::fromUtf8("UDP 9090 绑定失败，无法接收探针数据。\n请检查端口占用或权限。"));
        }
    }
    m_timer->start(33); 
}

void LogicAnalyzerView::stopMonitor() {
    if (m_udpSocket) {
        m_udpSocket->close();
        m_udpSocket->deleteLater();
        m_udpSocket = nullptr;
    }
    m_timer->stop();
}

void LogicAnalyzerView::onReadyReadUdp() {
    if(!m_udpSocket || !m_udpBindOk) return;
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());
        
        QString msg = QString::fromUtf8(datagram);
        QString channel;
        int val = 0;
        int ownerPid = -1;

        // 兼容格式1：channel:value （旧版）
        // 兼容格式2：pid|channel:value （推荐）
        QString payload = msg.trimmed();
        int pipePos = payload.indexOf('|');
        if (pipePos > 0) {
            bool okPid = false;
            int pidCandidate = payload.left(pipePos).toInt(&okPid);
            if (okPid) {
                ownerPid = pidCandidate;
                payload = payload.mid(pipePos + 1);
            }
        }

        QStringList parts = payload.split(':');
        if (parts.size() == 2) {
            channel = parts[0].trimmed();
            val = parts[1].trimmed().toInt();
            updateChannelFromProbe(channel, ownerPid);
            if(m_plotWidget) m_plotWidget->pushSample(channel, val);
        }
    }
}

// ==========================================
// 2. SysLoadWidget: 实时读取板子 /proc 节点展示负载
// ==========================================
SysLoadWidget::SysLoadWidget(QWidget* parent) : QWidget(parent) {
    m_lastTotalUser = m_lastTotalUserLow = m_lastTotalSys = m_lastTotalIdle = 0;
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(20);
    
    QLabel* title = new QLabel(QString::fromUtf8("板载核心负载与内存监视 (实时)"), this);
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #444;");
    mainLayout->addWidget(title, 0, Qt::AlignHCenter);
    
    QLabel* cpuLabel = new QLabel("CPU 动态主频占比:", this);
    cpuLabel->setStyleSheet("font-weight: bold;");
    m_cpuBar = new QProgressBar(this);
    m_cpuBar->setRange(0, 100);
    m_cpuBar->setTextVisible(true);
    m_cpuBar->setStyleSheet("QProgressBar { background-color: #EEE; border: 1px solid #CCC; border-radius: 5px; height: 25px; }"
                            "QProgressBar::chunk { background-color: #ff5c5c; border-radius: 4px; }");
    
    QLabel* memLabel = new QLabel("RAM 动态消耗:", this);
    memLabel->setStyleSheet("font-weight: bold;");
    m_memBar = new QProgressBar(this);
    m_memBar->setRange(0, 100);
    m_memBar->setStyleSheet("QProgressBar { background-color: #EEE; border: 1px solid #CCC; border-radius: 5px; height: 25px; }"
                            "QProgressBar::chunk { background-color: #5c8aff; border-radius: 4px; }");
                            
    m_detailsLabel = new QLabel(this);
    m_detailsLabel->setStyleSheet("color: #666; font-size: 13px;");
                            
    mainLayout->addWidget(cpuLabel);
    mainLayout->addWidget(m_cpuBar);
    mainLayout->addWidget(memLabel);
    mainLayout->addWidget(m_memBar);
    mainLayout->addWidget(m_detailsLabel);
    mainLayout->addStretch();
    
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SysLoadWidget::updateSysInfo);
    m_timer->start(1000); 
    updateSysInfo();
}

void SysLoadWidget::updateSysInfo() {
    QFile statFile("/proc/stat");
    if (statFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&statFile);
        QString line = in.readLine(); 
        if(line.startsWith("cpu ")) {
            QStringList tokens = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            if(tokens.size() > 4) {
                long long user = tokens[1].toLongLong();
                long long nice = tokens[2].toLongLong();
                long long system = tokens[3].toLongLong();
                long long idle = tokens[4].toLongLong();
                
                long long total = user + nice + system + idle;
                long long lastTotal = m_lastTotalUser + m_lastTotalUserLow + m_lastTotalSys + m_lastTotalIdle;
                long long totalDiff = total - lastTotal;
                long long idleDiff = idle - m_lastTotalIdle;
                
                double cpuUsage = 0.0;
                if(totalDiff > 0) {
                    cpuUsage = (totalDiff - idleDiff) * 100.0 / totalDiff;
                }
                
                m_cpuBar->setValue((int)cpuUsage);
                m_cpuBar->setFormat(QString("%1%").arg(cpuUsage, 0, 'f', 1));
                
                m_lastTotalUser = user;
                m_lastTotalUserLow = nice;
                m_lastTotalSys = system;
                m_lastTotalIdle = idle;
            }
        }
        statFile.close();
    }
    
    QFile memFile("/proc/meminfo");
    long memTotal = 0, memFree = 0, memAvailable = 0;
    if (memFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&memFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(QRegExp("\\s+"));
            if(parts.size() >= 2) {
                if (line.startsWith("MemTotal:")) memTotal = parts[1].toLong();
                else if (line.startsWith("MemFree:")) memFree = parts[1].toLong();
                else if (line.startsWith("MemAvailable:")) memAvailable = parts[1].toLong();
            }
        }
        memFile.close();
        
        long used = memTotal - (memAvailable > 0 ? memAvailable : memFree);
        if (memTotal > 0) {
            int memUsage = used * 100 / memTotal;
            m_memBar->setValue(memUsage);
            m_memBar->setFormat(QString("%1% (%2 MB / %3 MB)").arg(memUsage).arg(used/1024).arg(memTotal/1024));
            
            m_detailsLabel->setText(QString::fromUtf8("节点: /proc/stat; /proc/meminfo\n剩余空闲可用内存: %1 MB").arg((memAvailable > 0 ? memAvailable : memFree)/1024));
        }
    }
}

// ==========================================
// 3. 主界面 (集成监控模块)
// ==========================================
HardwareMonitorPage::HardwareMonitorPage(QWidget *parent) : IAppModule(parent) {
    // 强制白色底层。如果有无形组件这将会确保子程序展开不会有透墙黑洞
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("HardwareMonitorPage { background-color: #ffffff; }");
    this->setObjectName("HardwareMonitorPage");

    setupUI();
}

HardwareMonitorPage::~HardwareMonitorPage() {
    stopService();
}

void HardwareMonitorPage::stopService() {
    if (m_logicAnalyzer) m_logicAnalyzer->stopMonitor();
}

void HardwareMonitorPage::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 顶部伪状态栏 (白底统一样式)
    QWidget* topBar = new QWidget(this);
    topBar->setFixedHeight(40);
    topBar->setStyleSheet("background-color: #f7f7f7; border-bottom: 1px solid #dcdcdc;");
    QHBoxLayout* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 0, 10, 0);
    
    QPushButton* closeBtn = new QPushButton(topBar);
    closeBtn->setFixedSize(20, 20);
    closeBtn->setStyleSheet("QPushButton { border-image: url(:/res/images/hardware/close_new.png); background: transparent; }");
    connect(closeBtn, &QPushButton::clicked, this, &IAppModule::requestClose);
    
    QPushButton* minBtn = new QPushButton(topBar);
    minBtn->setFixedSize(20, 20);
    minBtn->setStyleSheet("QPushButton { border-image: url(:/res/images/hardware/minimize_new.png); background: transparent; }");
    connect(minBtn, &QPushButton::clicked, this, &IAppModule::requestMinimize);
    
    topLayout->addWidget(closeBtn);
    topLayout->addSpacing(10);
    topLayout->addWidget(minBtn);
    topLayout->addStretch();
    
    QLabel* titleLabel = new QLabel(QString::fromUtf8(" 硬件/外部层探针系统"), topBar);
    titleLabel->setStyleSheet("color: #333; font-weight: bold;");
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    
    mainLayout->addWidget(topBar);
    
    // 中部：侧边栏 + 工作区
    QWidget* contentWidget = new QWidget(this);
    QHBoxLayout* contentLayout = new QHBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    
    m_sideMenu = new QListWidget(this);
    // 这里缩小尺寸 160 -> 130，为主窗口节省安全空间保证小屏IMX6ULL全屏不会被撑破
    m_sideMenu->setFixedWidth(130);
    m_sideMenu->setStyleSheet("QListWidget { background-color: #fafafa; border-right: 1px solid #ddd; outline: none; }"
                              "QListWidget::item { padding: 15px 10px; font-size: 14px; border-bottom: 1px solid #eee; color: #333; }"
                              "QListWidget::item:selected { background-color: #e6f0ff; color: #0055ff; border-right: 3px solid #0055ff; }");
    
    m_sideMenu->addItem(QString::fromUtf8("树级探针台"));
    m_sideMenu->addItem(QString::fromUtf8("系统级占用"));
    
    m_stackWidget = new QStackedWidget(this);
    
    m_logicAnalyzer = new LogicAnalyzerView(this);
    m_logicAnalyzer->startMonitor();
    
    m_sysLoadInfo = new SysLoadWidget(this);
    
    m_stackWidget->addWidget(m_logicAnalyzer);
    m_stackWidget->addWidget(m_sysLoadInfo);
    
    connect(m_sideMenu, &QListWidget::currentRowChanged, m_stackWidget, &QStackedWidget::setCurrentIndex);
    m_sideMenu->setCurrentRow(0); // 默认开启探针页
    
    contentLayout->addWidget(m_sideMenu);
    contentLayout->addWidget(m_stackWidget, 1);
    
    mainLayout->addWidget(contentWidget, 1);
}
