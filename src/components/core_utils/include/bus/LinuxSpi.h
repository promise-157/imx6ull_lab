#ifndef LINUXSPI_H
#define LINUXSPI_H

#include <QString>
#include <QByteArray>
#include <linux/spi/spidev.h>

class LinuxSpi {
public:
    LinuxSpi();
    ~LinuxSpi();

    bool openDevice(const QString& devPath, uint8_t mode = SPI_MODE_0, uint32_t speed = 500000, uint8_t bits = 8);
    void closeDevice();
    bool isOpen() const;

    QByteArray transfer(const QByteArray& txData);

private:
    int m_fd;
    QString m_devPath;
    uint32_t m_speed;
    uint8_t m_mode;
    uint8_t m_bits;
};

#endif // LINUXSPI_H
