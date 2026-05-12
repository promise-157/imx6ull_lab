#include "bus/LinuxI2c.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <QDebug>

LinuxI2c::LinuxI2c() : m_fd(-1), m_addr(0) {}

LinuxI2c::~LinuxI2c() { closeDevice(); }

bool LinuxI2c::openDevice(const QString& devPath, int addr) {
    if (m_fd >= 0) closeDevice();
    m_fd = open(devPath.toLocal8Bit().constData(), O_RDWR);
    if (m_fd < 0) {
        qWarning() << "Failed to open I2C device:" << devPath;
        return false;
    }
    if (ioctl(m_fd, I2C_SLAVE, addr) < 0) {
        qWarning() << "Failed to set I2C slave address:" << addr;
        closeDevice();
        return false;
    }
    m_devPath = devPath;
    m_addr = addr;
    return true;
}

void LinuxI2c::closeDevice() {
    if (m_fd >= 0) {
        close(m_fd);
        m_fd = -1;
    }
}

bool LinuxI2c::isOpen() const { return m_fd >= 0; }

int LinuxI2c::writeData(int reg, const QByteArray& data) {
    if (!isOpen()) return -1;
    QByteArray buffer;
    buffer.append(static_cast<char>(reg));
    buffer.append(data);
    return write(m_fd, buffer.data(), buffer.size());
}

QByteArray LinuxI2c::readData(int reg, int length) {
    if (!isOpen()) return QByteArray();
    char regBuf = static_cast<char>(reg);
    if (write(m_fd, &regBuf, 1) != 1) return QByteArray();
    
    QByteArray buffer;
    buffer.resize(length);
    int ret = read(m_fd, buffer.data(), length);
    if (ret < 0) return QByteArray();
    buffer.resize(ret);
    return buffer;
}
