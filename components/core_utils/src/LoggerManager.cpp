#include "LoggerManager.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QTextStream>
#include <iostream>

LoggerManager &LoggerManager::instance() {
  static LoggerManager _instance;
  return _instance;
}

LoggerManager::~LoggerManager() {
  if (m_logFile.isOpen()) {
    m_logFile.close();
  }
}

void LoggerManager::init(const QString &logDirPath) {
  if (m_isInitialized)
    return;

  QDir dir(logDirPath);
  if (!dir.exists()) {
    dir.mkpath("."); // 如果没有 logs 文件夹，自动创建
  }

  // 每天生成一个新的 log 文件 (按天轮转)
  QString dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");
  QString filePath = dir.absoluteFilePath(QString("app_%1.log").arg(dateStr));

  m_logFile.setFileName(filePath);
  // Append 模式：接着文件末尾写，不清空原有内容
  if (m_logFile.open(QIODevice::WriteOnly | QIODevice::Append |
                     QIODevice::Text)) {
    m_isInitialized = true;
    // 把 Qt 自带的打印函数勾（Hook）到我们自定义的函数上
    qInstallMessageHandler(LoggerManager::messageHandler);
    qInfo() << "=============================================";
    qInfo() << " Logger Initialized. File:" << filePath;
    qInfo() << "=============================================";
  } else {
    std::cerr << "Failed to open log file!" << std::endl;
  }
}

void LoggerManager::messageHandler(QtMsgType type,
                                   const QMessageLogContext &context,
                                   const QString &msg) {
  LoggerManager &logger = LoggerManager::instance();
  QMutexLocker locker(&logger.m_mutex); // 保证多线程写文件安全

  // 1. 格式化时间戳
  QString timeStr =
      QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");

  // 2. 格式化日志级别
  QString levelStr;
  switch (type) {
  case QtDebugMsg:
    levelStr = "[DEBUG]";
    break;
  case QtInfoMsg:
    levelStr = "[INFO ]";
    break;
  case QtWarningMsg:
    levelStr = "[WARN ]";
    break;
  case QtCriticalMsg:
    levelStr = "[ERROR]";
    break;
  case QtFatalMsg:
    levelStr = "[FATAL]";
    break;
  }

  // 3. 提取调用打印的文件名 (去掉冗长的绝对路径)
  QString file = QString(context.file ? context.file : "Unknown");
  int lastSlash = file.lastIndexOf('/');
  if (lastSlash != -1)
    file = file.mid(lastSlash + 1);

  QString line = QString::number(context.line ? context.line : 0);

  // 4. 拼装最终的日志字符串: "2026-04-20 12:00:00.123 [INFO ] main.cpp:45 -
  // Hello World"
  QString logMessage =
      QString("%1 %2 %3:%4 - %5").arg(timeStr, levelStr, file, line, msg);

  // 5. 打印到终端控制台 (开发看)
  std::cout << logMessage.toLocal8Bit().constData() << std::endl;

  // 6. 写入到文件并立刻落盘 (溯源看)
  if (logger.m_isInitialized && logger.m_logFile.isOpen()) {
    QTextStream out(&logger.m_logFile);
    out << logMessage << "\n";
    out.flush(); // 强制落盘，防止断电丢失
  }
}