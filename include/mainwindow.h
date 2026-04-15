#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QStackedWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QList>
#include "LogManager.h"
#include "MusicPage.h"
#include "VideoPage.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void handleKeyClicked();
    void goHome() { stack->setCurrentIndex(0); }
    void goTerm() { stack->setCurrentIndex(1); }

private:
    QStackedWidget *stack;
    QPlainTextEdit *terminalDisplay;
    LogManager *logger;
    MusicPage *m_musicPage; // 明确持有指针
    VideoPage *m_videoPage;
    bool m_isShifted = false;
    QList<QPushButton*> m_letterButtons;

    QWidget* createDesktopPage();   
    QWidget* createTerminalPage();  
    QWidget* createSimpleAppPage(QString name); 
    void setupTopBar(QVBoxLayout *layout);
    QHBoxLayout* createKeyboardRow(const QStringList &low, const QStringList &up);
    void updateKeyboardDisplay();
};
#endif