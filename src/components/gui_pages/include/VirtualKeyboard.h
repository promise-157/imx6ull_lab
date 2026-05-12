#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H

#include <QHBoxLayout>
#include <QList>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class VirtualKeyboard : public QWidget {
  Q_OBJECT
public:
  static VirtualKeyboard &instance();

  // 拦截全局焦点事件
  bool eventFilter(QObject *watched, QEvent *event) override;

protected:
  void hideEvent(QHideEvent *event) override;

private:
  explicit VirtualKeyboard(QWidget *parent = nullptr);
  virtual ~VirtualKeyboard() = default;

  QHBoxLayout *createKeyboardRow(const QStringList &low, const QStringList &up);
  void updateKeyboardDisplay();
  void sendKey(int key, Qt::KeyboardModifiers mods, const QString &text);

private slots:
  void handleKeyClicked();
  void hideKeyboard();

private:
  QWidget *m_targetWidget;
  bool m_isShifted;
  QList<QPushButton *> m_letterButtons;
};

#endif // VIRTUALKEYBOARD_H