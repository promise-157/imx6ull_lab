// LogManager.h
#ifndef LOGMANAGER_H
#define LOGMANAGER_H
#include <QObject>
#include <QProcess>
#include <QProcessEnvironment>

class LogManager : public QObject {
    Q_OBJECT
public:
    explicit LogManager(QObject *parent = nullptr);
    void start();
    void writeCommand(const QString &cmd);
signals:
    void newLogReady(QString msg);
private slots:
    void onReadyRead();
private:
    QProcess *m_shell;
};
#endif