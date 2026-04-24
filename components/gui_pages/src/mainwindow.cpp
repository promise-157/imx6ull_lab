#include "mainwindow.h"
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QScrollBar>
#include <QStyle>
#include <QTextCursor>
#include <QTimer>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  this->setObjectName("MainWindow");

  QFile file(":/res/style/mainwindow.qss");
  if (file.open(QFile::ReadOnly)) {
    this->setStyleSheet(QLatin1String(file.readAll()));
    file.close();
  }

  // 整体为主垂直布局，上边是系统顶部栏，下边是页面堆栈
  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
  mainLayout->setContentsMargins(0, 0, 0, 0); // 无边距

  // 1. 初始化系统顶部全局栏
  setupSystemTopBar(mainLayout);

  // 2. 初始化堆栈容器
  m_stack = new QStackedWidget(this);
  mainLayout->addWidget(m_stack, 1);

  logger = new LogManager(this);

  // 添加桌面页和终端页
  m_stack->addWidget(createDesktopPage());  // index 0
  m_stack->addWidget(createTerminalPage()); // index 1

  logger->start();
  QTimer::singleShot(500, this, [this]() { logger->writeCommand("\n"); });
}

void MainWindow::setupSystemTopBar(QVBoxLayout *mainLayout) {
  m_systemTopBar = new QWidget(this);
  m_systemTopBar->setObjectName("SystemTopBar");

  QHBoxLayout *topBar = new QHBoxLayout(m_systemTopBar);
  topBar->setContentsMargins(10, 5, 10, 5);

  QPushButton *btnHome = new QPushButton("桌面", this);
  btnHome->setObjectName("TopHomeBtn");
  connect(btnHome, &QPushButton::clicked, this, &MainWindow::goHome);

  QPushButton *btnTerm = new QPushButton("终端", this);
  btnTerm->setObjectName("TopTermBtn");
  connect(btnTerm, &QPushButton::clicked, this, &MainWindow::goTerm);

  topBar->addWidget(btnHome);
  topBar->addWidget(btnTerm);

  // 中间的后台任务栏
  m_taskBarLayout = new QHBoxLayout();
  m_taskBarLayout->setSpacing(10);
  topBar->addLayout(m_taskBarLayout);

  topBar->addStretch();

  QPushButton *btnExit = new QPushButton("✖ 退出系统", this);
  btnExit->setObjectName("TopExitBtn");
  connect(btnExit, &QPushButton::clicked, qApp, &QApplication::quit);
  topBar->addWidget(btnExit);

  mainLayout->addWidget(m_systemTopBar);
}

void MainWindow::registerApp(IAppModule *app) {
  if (!app)
    return;

  m_registeredApps.append(app);
  m_stack->addWidget(app);
  int appIndex = m_stack->indexOf(app);

  // 在桌面上创建该应用的快捷方式
  QPushButton *iconBtn = new QPushButton(app->appName());
  iconBtn->setFixedSize(
      110,
      110); // 缩小图标尺寸以适应IMX6ULL常见的分辨率(480x272等)，防止撑爆屏幕导致不能全屏
  iconBtn->setObjectName("DesktopIconBtn");
  if (!app->appIconPath().isEmpty()) {
    iconBtn->setIcon(QIcon(app->appIconPath()));
    iconBtn->setIconSize(QSize(52, 52));
  }

  // 点击图标触发：打开应用并切换焦点
  connect(iconBtn, &QPushButton::clicked, this, [this, appIndex, app]() {
    m_stack->setCurrentIndex(appIndex);
    addAppToTaskBar(app);
  });

  int count = m_registeredApps.size() - 1;
  // 每行最多两个以适应窄屏 (原来是 count % 4，现在改成 count % 3 甚至 2)
  m_desktopGrid->addWidget(iconBtn, count / 3, count % 3, Qt::AlignCenter);

  // 接管应用退后台和彻底关闭的统一信号
  connect(app, &IAppModule::requestMinimize, this,
          [this, app]() { onAppMinimize(app); });
  connect(app, &IAppModule::requestClose, this,
          [this, app]() { onAppClose(app); });
}

void MainWindow::onAppMinimize(IAppModule *app) { goHome(); }

void MainWindow::onAppClose(IAppModule *app) {
  goHome();
  removeAppFromTaskBar(app);
  app->stopService();
}

void MainWindow::addAppToTaskBar(IAppModule *app) {
  if (m_runningApps.contains(app))
    return;

  QPushButton *taskBtn = new QPushButton("后台:" + app->appName());
  taskBtn->setFixedSize(120, 40);
  taskBtn->setObjectName("TaskBarBtn");

  connect(taskBtn, &QPushButton::clicked, this,
          [this, app]() { m_stack->setCurrentWidget(app); });

  m_taskBarLayout->addWidget(taskBtn);
  m_runningApps.insert(app, taskBtn);
}

void MainWindow::removeAppFromTaskBar(IAppModule *app) {
  if (!m_runningApps.contains(app))
    return;

  QPushButton *taskBtn = m_runningApps.value(app);
  m_taskBarLayout->removeWidget(taskBtn);
  taskBtn->deleteLater();
  m_runningApps.remove(app);
}

QWidget *MainWindow::createDesktopPage() {
  QWidget *page = new QWidget();
  page->setObjectName("DesktopPage");
  QVBoxLayout *layout = new QVBoxLayout(page);

  // 创建一个居中的桌面网格
  m_desktopGrid = new QGridLayout();
  m_desktopGrid->setSpacing(20);
  m_desktopGrid->setContentsMargins(20, 20, 20, 20);

  layout->addLayout(m_desktopGrid);
  layout->addStretch();
  return page;
}

QWidget *MainWindow::createTerminalPage() {
  QWidget *page = new QWidget();
  page->setObjectName("TerminalPage");
  QVBoxLayout *layout = new QVBoxLayout(page);
  layout->setContentsMargins(0, 0, 0, 0);

  terminalDisplay = new QPlainTextEdit();
  terminalDisplay->setObjectName("TerminalDisplay");
  terminalDisplay->setReadOnly(true);
  terminalDisplay->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  layout->addWidget(terminalDisplay, 1);

  QWidget *kbContainer = new QWidget();
  kbContainer->setObjectName("KeyboardContainer");
  QVBoxLayout *kbLayout = new QVBoxLayout(kbContainer);
  kbLayout->setContentsMargins(5, 5, 5, 5);
  kbLayout->setSpacing(4);

  kbLayout->addLayout(
      createKeyboardRow({"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"},
                        {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"}));
  kbLayout->addLayout(
      createKeyboardRow({"a", "s", "d", "f", "g", "h", "j", "k", "l"},
                        {"!", "@", "#", "$", "%", "&", "*", "(", ")"}));
  kbLayout->addLayout(createKeyboardRow(
      {"Shift", "z", "x", "c", "v", "b", "n", "m", ",", ".", "DEL"},
      {"Shift", "Z", "X", "C", "V", "B", "N", "M", "?", "\"", "DEL"}));
  kbLayout->addLayout(
      createKeyboardRow({"Space", "/", "-", "_", ":", "Enter"},
                        {"Space", "'", "|", ";", "=", "Enter"}));

  layout->addWidget(kbContainer);

  connect(logger, &LogManager::newLogReady, this, [this](QString msg) {
    QTextCursor cursor = terminalDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    int delCount = msg.count('\b') + msg.count("[K") + msg.count(QChar(127));
    for (int i = 0; i < delCount; ++i) {
      if (!cursor.atBlockStart())
        cursor.deletePreviousChar();
    }
    msg.replace("\b", "");
    msg.replace("[K", "");
    msg.replace(QString(QChar(127)), "");
    if (!msg.isEmpty()) {
      terminalDisplay->insertPlainText(msg);
      terminalDisplay->moveCursor(QTextCursor::End);
    }
  });
  return page;
}

QHBoxLayout *MainWindow::createKeyboardRow(const QStringList &low,
                                           const QStringList &up) {
  QHBoxLayout *h = new QHBoxLayout();
  h->setSpacing(4);
  for (int i = 0; i < low.size(); ++i) {
    QPushButton *b = new QPushButton(low[i]);
    b->setProperty("low", low[i]);
    b->setProperty("up", up[i]);
    b->setMinimumHeight(60);

    b->setObjectName("KeyboardBtn");
    if (low[i] == "Enter")
      b->setObjectName("KeyboardBtnEnter");
    if (low[i] == "DEL")
      b->setObjectName("KeyboardBtnDel");
    if (low[i] == "Shift") {
      b->setObjectName("KeyboardBtnShift");
      b->setProperty("isShifted", false);
    }

    m_letterButtons.append(b);
    connect(b, &QPushButton::clicked, this, &MainWindow::handleKeyClicked);
    h->addWidget(b);

    if (low[i] == "Space")
      h->setStretch(i, 4);
    else if (low[i] == "Enter")
      h->setStretch(i, 2);
    else
      h->setStretch(i, 1);
  }
  return h;
}

void MainWindow::updateKeyboardDisplay() {
  for (QPushButton *b : m_letterButtons) {
    b->setText(m_isShifted ? b->property("up").toString()
                           : b->property("low").toString());
  }
}

void MainWindow::handleKeyClicked() {
  QPushButton *btn = qobject_cast<QPushButton *>(sender());
  if (!btn)
    return;
  QString val = btn->text();

  if (val == "Shift") {
    m_isShifted = !m_isShifted;
    btn->setProperty("isShifted", m_isShifted);
    btn->style()->unpolish(btn);
    btn->style()->polish(btn);
    updateKeyboardDisplay();
  } else if (val == "Enter") {
    logger->writeCommand("\n");
  } else if (val == "DEL") {
    logger->writeCommand("\x7f");
  } else if (val == "Space") {
    logger->writeCommand(" ");
  } else {
    logger->writeCommand(val);
  }
}
