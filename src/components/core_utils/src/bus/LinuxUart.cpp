#include "bus/LinuxUart.h"
#include <QDebug>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

LinuxUart::LinuxUart() : m_fd(-1) {}
LinuxUart::~LinuxUart() { closeDevice(); }

bool LinuxUart::openDevice(const QString& devPath, int baudRate) {
    if (isOpen()) closeDevice();
    m_devPath = devPath;
    
    // O_NOCTTY 不作为控制终端, O_NDELAY 非阻塞
    m_fd = ::open(devPath.toLocal8Bit().constData(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (m_fd < 0) {
        qWarning() << "[LinuxUart] Failed to open" << devPath;
        return false;
    }

    struct termios options;
    tcgetattr(m_fd, &options);
    
    // 默认配置 8N1
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE; 
    options.c_cflag |= CS8;   // 8 bits
    options.c_cflag &= ~PARENB; // No Parity
    options.c_cflag &= ~CSTOPB; // 1 stop bit
    
    // 原始模式 Raw mode
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    
    int baudFlag = B115200;
    if(baudRate == 9600) baudFlag = B9600;
    else if(baudRate == 19200) baudFlag = B19200;
    else if(baudRate == 38400) baudFlag = B38400;
    else if(baudRate == 115200) baudFlag = B115200;
    
    cfsetispeed(&options, baudFlag);
    cfsetospeed(&options, baudFlag);
    
    tcflush(m_fd, TCIFLUSH);
    tcsetattr(m_fd, TCSANOW, &options);
    
    // 还原为阻塞以便后续应用层select控制（通常QT的话可用QSocketNotifier或轮询线程）
    fcntl(m_fd, F_SETFL, 0); 
    
    qInfo() << "[LinuxUart] Opened" << devPath << "@" << baudRate;
    return true;
}

void LinuxUart::closeDevice() {
    if(m_fd >= 0) {
        ::close(m_fd);
        m_fd = -1;
        qInfo() << "[LinuxUart] Closed" << m_devPath;
    }
}

bool LinuxUart::isOpen() const { return m_fd >= 0; }

int LinuxUart::writeData(const QByteArray& data) {
    if (!isOpen()) return -1;
    return ::write(m_fd, data.constData(), data.size());
}

QByteArray LinuxUart::readData(int maxLen) {
    if (!isOpen()) return QByteArray();
    char* buf = new char[maxLen];
    
    // 我们在此简单设为非阻塞模式读取一次
    int oldFlags = fcntl(m_fd, F_GETFL, 0);
    fcntl(m_fd, F_SETFL, oldFlags | O_NONBLOCK);
    
    int ret = ::read(m_fd, buf, maxLen);
    
    fcntl(m_fd, F_SETFL, oldFlags);
    
    QByteArray res;
    if (ret > 0) res = QByteArray(buf, ret);
    delete[] buf;
    return res;
}
