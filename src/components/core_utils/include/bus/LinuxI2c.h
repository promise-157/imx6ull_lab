#ifndef LINUXI2C_H
#define LINUXI2C_H

#include <QString>
#include <QByteArray>

class LinuxI2c {
public:
    LinuxI2c();
    ~LinuxI2c();

    bool openDevice(const QString& devPath, int addr);
    void closeDevice();
    bool isOpen() const;

    int writeData(int reg, const QByteArray& data);
    QByteArray readData(int reg, int length);

private:
    int m_fd;
    QString m_devPath;
    int m_addr;
};

#endif // LINUXI2C_H
