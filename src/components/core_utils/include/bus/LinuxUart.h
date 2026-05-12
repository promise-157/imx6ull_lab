#ifndef LINUXUART_H
#define LINUXUART_H

#include <QString>
#include <QByteArray>

class LinuxUart {
public:
    LinuxUart();
    ~LinuxUart();

    bool openDevice(const QString& devPath, int baudRate = 115200);
    void closeDevice();
    bool isOpen() const;

    // 非阻塞发送
    int writeData(const QByteArray& data);
    // 非阻塞读取一段
    QByteArray readData(int maxLen = 1024);

private:
    int m_fd;
    QString m_devPath;
};

#endif // LINUXUART_H
