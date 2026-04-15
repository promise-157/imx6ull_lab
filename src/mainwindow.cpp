#include "mainwindow.h"
#include <QLabel>
#include <QGridLayout>
#include <QTimer>
#include <QTextCursor>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    stack = new QStackedWidget(this);
    logger = new LogManager(this);

    stack->addWidget(createDesktopPage());   // 0
    stack->addWidget(createTerminalPage());  // 1
    // 增量添加新页面
    m_musicPage = new MusicPage(this);
    stack->addWidget(m_musicPage);           // Index 2
    m_videoPage= new VideoPage(this);
    stack->addWidget(m_videoPage); // Index 3
 //   for(int i=1; i<=4; ++i) stack->addWidget(createSimpleAppPage(QString("应用 %1").arg(i)));

    setCentralWidget(stack);
    logger->start();
    QTimer::singleShot(500, this, [this](){ logger->writeCommand("\n"); });
}

void MainWindow::setupTopBar(QVBoxLayout *layout) {
    QHBoxLayout *topBar = new QHBoxLayout();
    topBar->setContentsMargins(10, 5, 10, 5);
    QString style = "QPushButton { background: #333; color: white; border-radius: 5px; padding: 10px; font-weight: bold; min-width: 90px; }";
    
    QPushButton *btnHome = new QPushButton("桌面", this);
    btnHome->setStyleSheet(style);
    connect(btnHome, &QPushButton::clicked, this, &MainWindow::goHome);
    
    QPushButton *btnTerm = new QPushButton("终端", this);
    btnTerm->setStyleSheet(style);
    connect(btnTerm, &QPushButton::clicked, this, &MainWindow::goTerm);

    topBar->addWidget(btnHome);
    topBar->addWidget(btnTerm);
    topBar->addStretch();
    
    QPushButton *btnExit = new QPushButton("✖ 退出", this);
    btnExit->setStyleSheet("background: #800; color: white; border-radius: 5px; padding: 10px; font-weight: bold;");
    connect(btnExit, &QPushButton::clicked, qApp, &QApplication::quit);
    topBar->addWidget(btnExit);

    layout->addLayout(topBar);
}

QWidget* MainWindow::createDesktopPage() {
    QWidget *page = new QWidget();
    page->setStyleSheet("background-color: #1a1a1a;");
    QVBoxLayout *layout = new QVBoxLayout(page);
    
    // 桌面也要有 TopBar，这样才能从桌面点“终端”或者“退出”
    setupTopBar(layout); 

    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(30);

    // 假设你有 2 个真正的 App
    QStringList apps = {"音乐播放器", "视频播放器"};

    for(int i=0; i<apps.size(); ++i) {
        QPushButton *btn = new QPushButton(apps[i]);
        btn->setFixedSize(160, 160);
        btn->setStyleSheet("QPushButton { background: #2c3e50; color: white; border-radius: 20px; font-size: 18px; }");
        
        // 跳转逻辑：
        connect(btn, &QPushButton::clicked, this, [this, i](){
            if(i == 0) stack->setCurrentIndex(2); // 点击音乐图标 -> 跳转到 MusicPage
            else if(i == 1) stack->setCurrentIndex(3); // 点击视频图标 -> 跳转到 VideoPage
        });
        
        grid->addWidget(btn, i/2, i%2, Qt::AlignCenter);
    }
    layout->addLayout(grid);
    layout->addStretch();
    return page;
}

QWidget* MainWindow::createTerminalPage() {
    QWidget *page = new QWidget();
    page->setStyleSheet("background-color: #000;");
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    setupTopBar(layout);

    terminalDisplay = new QPlainTextEdit();
    terminalDisplay->setReadOnly(true);
    // 强制开启 30px 宽的滚动条，方便触摸
    terminalDisplay->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    terminalDisplay->setStyleSheet(
        "QPlainTextEdit { background: black; color: #00FF00; font-family: 'Monospace'; font-size: 16px; border: none; padding: 10px; }"
        "QScrollBar:vertical { width: 30px; background: #222; }"
        "QScrollBar::handle:vertical { background: #555; border-radius: 5px; min-height: 40px; }"
    );
    layout->addWidget(terminalDisplay, 1);

    QWidget *kbContainer = new QWidget();
    kbContainer->setStyleSheet("background-color: #222;");
    QVBoxLayout *kbLayout = new QVBoxLayout(kbContainer);
    kbLayout->setContentsMargins(5, 5, 5, 5);
    kbLayout->setSpacing(4);

    // 第一行: 字母 / 数字
    kbLayout->addLayout(createKeyboardRow({"q","w","e","r","t","y","u","i","o","p"}, {"1","2","3","4","5","6","7","8","9","0"}));
    // 第二行: 字母 / 符号 (补齐了 &)
    kbLayout->addLayout(createKeyboardRow({"a","s","d","f","g","h","j","k","l"}, {"!","@","#","$","%","&","*","(",")"}));
    // 第三行: Shift + 字母 / 特殊符号
    kbLayout->addLayout(createKeyboardRow({"Shift","z","x","c","v","b","n","m",",",".","DEL"}, {"Shift","Z","X","C","V","B","N","M","?","\"","DEL"}));
    // 第四行: 功能键
    kbLayout->addLayout(createKeyboardRow({"Space", "/", "-", "_", ":", "Enter"}, {"Space", "'", "|", ";", "=", "Enter"}));
    
    layout->addWidget(kbContainer);

    connect(logger, &LogManager::newLogReady, this, [this](QString msg){
        QTextCursor cursor = terminalDisplay->textCursor();
        cursor.movePosition(QTextCursor::End);
        int delCount = msg.count('\b') + msg.count("[K") + msg.count(QChar(127));
        for(int i = 0; i < delCount; ++i) {
            if(!cursor.atBlockStart()) cursor.deletePreviousChar();
        }
        msg.replace("\b", ""); msg.replace("[K", ""); msg.replace(QString(QChar(127)), "");
        if(!msg.isEmpty()) {
            terminalDisplay->insertPlainText(msg);
            terminalDisplay->moveCursor(QTextCursor::End);
        }
    });
    return page;
}

QHBoxLayout* MainWindow::createKeyboardRow(const QStringList &low, const QStringList &up) {
    QHBoxLayout *h = new QHBoxLayout();
    h->setSpacing(4);
    for(int i=0; i<low.size(); ++i) {
        QPushButton *b = new QPushButton(low[i]);
        b->setProperty("low", low[i]);
        b->setProperty("up", up[i]);
        b->setMinimumHeight(60);
        
        QString style = "QPushButton { background: #444; color: white; border-radius: 6px; font-size: 18px; font-weight: bold; } QPushButton:pressed { background: #00FF00; color: black; }";
        if(low[i] == "Enter") style += "background: #1e8449;";
        if(low[i] == "DEL")   style += "background: #833;";
        if(low[i] == "Shift") style += "background: #2e5a88;";
        b->setStyleSheet(style);

        m_letterButtons.append(b);
        connect(b, &QPushButton::clicked, this, &MainWindow::handleKeyClicked);
        h->addWidget(b);
        
        // 固定比例分配宽度，避免畸形
        if(low[i] == "Space") h->setStretch(i, 4);
        else if(low[i] == "Enter") h->setStretch(i, 2);
        else h->setStretch(i, 1);
    }
    return h;
}

void MainWindow::updateKeyboardDisplay() {
    for(QPushButton* b : m_letterButtons) {
        b->setText(m_isShifted ? b->property("up").toString() : b->property("low").toString());
    }
}

void MainWindow::handleKeyClicked() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if(!btn) return;
    QString val = btn->text();

    if(val == "Shift") {
        m_isShifted = !m_isShifted;
        btn->setStyleSheet(m_isShifted ? "background: #3498db; color: white; border-radius: 6px; font-weight: bold;" 
                                       : "background: #2e5a88; color: white; border-radius: 6px; font-weight: bold;");
        updateKeyboardDisplay();
    } else if(val == "Enter") {
        logger->writeCommand("\n");
    } else if(val == "DEL") {
        logger->writeCommand("\x7f");
    } else if(val == "Space") {
        logger->writeCommand(" ");
    } else {
        logger->writeCommand(val);
    }
}

QWidget* MainWindow::createSimpleAppPage(QString name) {
    QWidget *page = new QWidget();
    page->setStyleSheet("background-color: #121212;");
    QVBoxLayout *layout = new QVBoxLayout(page);
    setupTopBar(layout);
    QLabel *label = new QLabel("正在运行: " + name);
    label->setStyleSheet("color: white; font-size: 24px;");
    layout->addStretch();
    layout->addWidget(label, 0, Qt::AlignCenter);
    QPushButton *btnBack = new QPushButton("返回桌面");
    btnBack->setFixedSize(200, 50);
    btnBack->setStyleSheet("background: #444; color: white; border-radius: 8px;");
    connect(btnBack, &QPushButton::clicked, this, &MainWindow::goHome);
    layout->addWidget(btnBack, 0, Qt::AlignCenter);
    layout->addStretch();
    return page;
}