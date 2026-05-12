#include "VirtualKeyboard.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QEvent>
#include <QKeyEvent>
#include <QScreen>
#include <QStyle>

VirtualKeyboard &VirtualKeyboard::instance() {
  static VirtualKeyboard kb;
  return kb;
}

VirtualKeyboard::VirtualKeyboard(QWidget *parent)
    : QWidget(parent), m_targetWidget(nullptr), m_isShifted(false) {
  // 配置为置顶工具窗口且无边框，不获取焦点，不抢占活动窗口
  setWindowFlags(Qt::Tool | Qt::FramelessWindowHint |
                 Qt::WindowDoesNotAcceptFocus | Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_ShowWithoutActivating);
  setObjectName("GlobalVirtualKeyboard");

  // 设置半透明背景或统一样式
  setStyleSheet("#GlobalVirtualKeyboard { background-color: #333333; "
                "border-top: 2px solid #555555; }"
                "QPushButton { background-color: #555555; color: white; "
                "border-radius: 4px; font-size: 18px; }"
                "QPushButton:pressed { background-color: #777777; }"
                "QPushButton#KeyboardBtnEnter { background-color: #1E90FF; }"
                "QPushButton#KeyboardBtnDel { background-color: #DC143C; }");

  QVBoxLayout *kbLayout = new QVBoxLayout(this);
  kbLayout->setContentsMargins(5, 5, 5, 5);
  kbLayout->setSpacing(4);

  // 顶部增加一个关闭按钮栏，方便手动隐藏
  QHBoxLayout *topLayout = new QHBoxLayout();
  topLayout->addStretch();
  QPushButton *closeBtn = new QPushButton("▼");
  closeBtn->setFixedSize(60, 30);
  closeBtn->setFocusPolicy(Qt::NoFocus);
  connect(closeBtn, &QPushButton::clicked, this,
          &VirtualKeyboard::hideKeyboard);
  topLayout->addWidget(closeBtn);
  kbLayout->addLayout(topLayout);

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

  setMinimumHeight(240); // 确保在屏幕下方占一定高度
}

QHBoxLayout *VirtualKeyboard::createKeyboardRow(const QStringList &low,
                                                const QStringList &up) {
  QHBoxLayout *h = new QHBoxLayout();
  h->setSpacing(4);
  for (int i = 0; i < low.size(); ++i) {
    QPushButton *b = new QPushButton(low[i]);
    b->setProperty("low", low[i]);
    b->setProperty("up", up[i]);
    b->setMinimumHeight(45);        // 相比原来的略小一点
    b->setFocusPolicy(Qt::NoFocus); // 点击时不能抢焦点！

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
    connect(b, &QPushButton::clicked, this, &VirtualKeyboard::handleKeyClicked);
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

void VirtualKeyboard::updateKeyboardDisplay() {
  for (QPushButton *b : m_letterButtons) {
    b->setText(m_isShifted ? b->property("up").toString()
                           : b->property("low").toString());
  }
}

void VirtualKeyboard::handleKeyClicked() {
  QPushButton *btn = qobject_cast<QPushButton *>(sender());
  if (!btn || !m_targetWidget)
    return;

  QString val = btn->text();

  if (val == "Shift") {
    m_isShifted = !m_isShifted;
    btn->setProperty("isShifted", m_isShifted);
    btn->style()->unpolish(btn);
    btn->style()->polish(btn);
    updateKeyboardDisplay();
  } else if (val == "Enter") {
    sendKey(Qt::Key_Return, Qt::NoModifier, "\r");
    hideKeyboard();
  } else if (val == "DEL") {
    sendKey(Qt::Key_Backspace, Qt::NoModifier, "");
  } else if (val == "Space") {
    sendKey(Qt::Key_Space, Qt::NoModifier, " ");
  } else {
    // 普通字符，假设英文字符
    sendKey(0, Qt::NoModifier, val);
  }
}

void VirtualKeyboard::sendKey(int key, Qt::KeyboardModifiers mods,
                              const QString &text) {
  if (!m_targetWidget)
    return;
  QKeyEvent pressEvent(QEvent::KeyPress, key, mods, text);
  QKeyEvent releaseEvent(QEvent::KeyRelease, key, mods, text);
  QCoreApplication::sendEvent(m_targetWidget, &pressEvent);
  QCoreApplication::sendEvent(m_targetWidget, &releaseEvent);
}

void VirtualKeyboard::hideKeyboard() {
  hide();
  m_targetWidget = nullptr;
}

void VirtualKeyboard::hideEvent(QHideEvent *event) {
  QWidget::hideEvent(event);
  m_targetWidget = nullptr;
}

bool VirtualKeyboard::eventFilter(QObject *watched, QEvent *event) {
  if (event->type() == QEvent::FocusIn) {
    QWidget *w = qobject_cast<QWidget *>(watched);
    if (w && (w->inherits("QLineEdit") || w->inherits("QTextEdit") ||
              w->inherits("QPlainTextEdit"))) {
      // 忽略只读框和本身是不需要弹键盘的特殊框（如TerminalDisplay）
      if (w->property("readOnly").toBool()) {
        return false;
      }
      if (w->objectName() == "TerminalDisplay") {
        return false;
      }

      m_targetWidget = w;

      // 动态计算在屏幕底部显示
      QScreen *screen = QGuiApplication::primaryScreen();
      QRect screenGeometry = screen->geometry();
      int kbHeight = this->minimumHeight();
      this->setGeometry(0, screenGeometry.height() - kbHeight,
                        screenGeometry.width(), kbHeight);

      this->show();
    }
  } else if (event->type() == QEvent::FocusOut) {
    // 焦点移出并且新焦点不是键盘自身上的拉取
    QWidget *w = qobject_cast<QWidget *>(watched);
    if (w == m_targetWidget) {
      // 防止点击键盘按钮时触发 FocusOut，键盘按钮设置了 NoFocus所以一般不会
      hideKeyboard();
    }
  }

  return QWidget::eventFilter(watched, event);
}