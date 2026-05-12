#include "TerminalPage.h"
#include <QStyle>
#include <QTextCursor>
#include <QTimer>

TerminalPage::TerminalPage(QWidget *parent) : IAppModule(parent) {
  this->setObjectName("TerminalPage");
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  // 返回和关闭按钮
  QHBoxLayout *topLayout = new QHBoxLayout();
  QPushButton *backBtn = new QPushButton("← 返回桌面", this);
  backBtn->setFixedSize(120, 40);
  connect(backBtn, &QPushButton::clicked, this, &IAppModule::requestMinimize);
  topLayout->addWidget(backBtn);

  QPushButton *closeBtn = new QPushButton("✕ 彻底关闭", this);
  closeBtn->setFixedSize(120, 40);
  // 可加上红色警告样式
  closeBtn->setStyleSheet("QPushButton { background-color: #ff4c4c; color: "
                          "white; border-radius: 5px; font-weight: bold; } "
                          "QPushButton:pressed { background-color: #cc0000; }");
  connect(closeBtn, &QPushButton::clicked, this, &IAppModule::requestClose);
  topLayout->addWidget(closeBtn);

  topLayout->addStretch();
  layout->addLayout(topLayout);

  terminalDisplay = new QPlainTextEdit(this);
  terminalDisplay->setObjectName("TerminalDisplay"); // 让全局键盘过滤它
  terminalDisplay->setReadOnly(true);
  terminalDisplay->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  layout->addWidget(terminalDisplay, 1);

  QWidget *kbContainer = new QWidget(this);
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

  logger = new LogManager(this);
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

  logger->start();
  QTimer::singleShot(500, this, [this]() { logger->writeCommand("\n"); });
}

TerminalPage::~TerminalPage() { stopService(); }

void TerminalPage::stopService() {
  if (logger) {
    // logger->stop(); // If needed
  }
}

QHBoxLayout *TerminalPage::createKeyboardRow(const QStringList &low,
                                             const QStringList &up) {
  QHBoxLayout *h = new QHBoxLayout();
  h->setSpacing(4);
  for (int i = 0; i < low.size(); ++i) {
    QPushButton *b = new QPushButton(low[i], this);
    b->setProperty("low", low[i]);
    b->setProperty("up", up[i]);
    b->setMinimumHeight(60);
    b->setFocusPolicy(Qt::NoFocus);

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
    connect(b, &QPushButton::clicked, this, &TerminalPage::handleKeyClicked);
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

void TerminalPage::updateKeyboardDisplay() {
  for (QPushButton *b : m_letterButtons) {
    b->setText(m_isShifted ? b->property("up").toString()
                           : b->property("low").toString());
  }
}

void TerminalPage::handleKeyClicked() {
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