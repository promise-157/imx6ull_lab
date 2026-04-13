// LogManager.cpp
#include "LogManager.h"

LogManager::LogManager(QObject *parent) : QObject(parent) {
    m_shell = new QProcess(this);
    m_shell->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_shell, &QProcess::readyReadStandardOutput, this, &LogManager::onReadyRead);
}

void LogManager::start() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("TERM", "vt100"); 
    env.insert("PS1", "[root@ATK-IMX6U \\w]# "); 
    m_shell->setProcessEnvironment(env);
    m_shell->start("/bin/sh", QStringList() << "-i");
    m_shell->write("stty echo erase ^? -icanon\n");
    m_shell->write("clear\n");
}

void LogManager::onReadyRead() {
    emit newLogReady(QString::fromLocal8Bit(m_shell->readAllStandardOutput()));
}

void LogManager::writeCommand(const QString &cmd) {
    if (m_shell->state() == QProcess::Running) {
        m_shell->write(cmd.toUtf8());
        m_shell->waitForBytesWritten();
    }
}