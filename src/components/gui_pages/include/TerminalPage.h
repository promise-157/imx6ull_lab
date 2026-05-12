#ifndef TERMINALPAGE_H
#define TERMINALPAGE_H

#include "IAppModule.h"
#include "LogManager.h"
#include <QHBoxLayout>
#include <QList>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

class TerminalPage : public IAppModule {
  Q_OBJECT
public:
  explicit TerminalPage(QWidget *parent = nullptr);
  ~TerminalPage() override;

  QString appName() const override { return "系统终端"; }
  QString appIconPath() const override {
    return "";
  } // 若有图标可以提供，目前用空让按钮显示文字
  void stopService() override;

private slots:
  void handleKeyClicked();

private:
  LogManager *logger;
  QPlainTextEdit *terminalDisplay;
  QList<QPushButton *> m_letterButtons;
  bool m_isShifted = false;

  QHBoxLayout *createKeyboardRow(const QStringList &low, const QStringList &up);
  void updateKeyboardDisplay();
};

#endif // TERMINALPAGE_H