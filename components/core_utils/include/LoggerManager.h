#ifndef LOGGERMANAGER_H
#define LOGGERMANAGER_H

#include <QString>
#include <QFile>
#include <QMutex>

/**
 * 【工业级万能日志模板 (基于 Qt)】
 * 适用场景：任何 Qt 项目。
 * 核心亮点：
 * 1. 你完全不需要改变以前写代码的习惯，继续用 qDebug(), qWarning(), qCritical() 即可！
 * 2. 它会在底层“拦截”打印，不但输出到终端，还会自动加时间戳、文件名、行号，写入到本地 log 文件。
 * 3. 线程安全（带锁），多线程同时写也不会崩溃。
 */
class LoggerManager {
public:
    static LoggerManager& instance();

    // 初始化日志系统，传入日志保管的目录（如 "./logs"）
    void init(const QString& logDirPath);

    // 给 Qt 底层回调使用的钩子函数，外部请勿直接调用
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
    LoggerManager() = default;
    ~LoggerManager();
    LoggerManager(const LoggerManager&) = delete;
    LoggerManager& operator=(const LoggerManager&) = delete;

    QFile m_logFile;
    QMutex m_mutex;
    bool m_isInitialized = false;
};

#endif // LOGGERMANAGER_H