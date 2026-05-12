#include "bus/LinuxSpi.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <QDebug>

LinuxSpi::LinuxSpi() : m_fd(-1), m_speed(500000), m_mode(0), m_bits(8) {}

LinuxSpi::~LinuxSpi() { closeDevice(); }

bool LinuxSpi::openDevice(const QString& devPath, uint8_t mode, uint32_t speed, uint8_t bits) {
    if (m_fd >= 0) closeDevice();
    m_fd = open(devPath.toLocal8Bit().constData(), O_RDWR);
    if (m_fd < 0) {
        qWarning() << "Failed to open SPI device:" << devPath;
        return false;
    }
    
    m_mode = mode;
    m_speed = speed;
    m_bits = bits;

    if (ioctl(m_fd, SPI_IOC_WR_MODE, &m_mode) < 0) goto fail;
    if (ioctl(m_fd, SPI_IOC_WR_BITS_PER_WORD, &m_bits) < 0) goto fail;
    if (ioctl(m_fd, SPI_IOC_WR_MAX_SPEED_HZ, &m_speed) < 0) goto fail;

    m_devPath = devPath;
    return true;

fail:
    qWarning() << "Failed to setup SPI device parameters.";
    closeDevice();
    return false;
}

void LinuxSpi::closeDevice() {
    if (m_fd >= 0) {
        close(m_fd);
        m_fd = -1;
    }
}

bool LinuxSpi::isOpen() const { return m_fd >= 0; }

QByteArray LinuxSpi::transfer(const QByteArray& txData) {
    if (!isOpen() || txData.isEmpty()) return QByteArray();
    
    QByteArray rxData;
    rxData.resize(txData.size());

    struct spi_ioc_transfer tr = {0};
    tr.tx_buf = (unsigned long)txData.constData();
    tr.rx_buf = (unsigned long)rxData.data();
    tr.len = txData.size();
    tr.speed_hz = m_speed;
    tr.bits_per_word = m_bits;

    if (ioctl(m_fd, SPI_IOC_MESSAGE(1), &tr) < 1) {
        qWarning() << "SPI transfer failed";
        return QByteArray();
    }
    return rxData;
}
