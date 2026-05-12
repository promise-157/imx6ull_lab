#include "bus/SocketCan.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <cstring>
#include <QDebug>

SocketCan::SocketCan() : m_socket(-1) {}

SocketCan::~SocketCan() { closeDevice(); }

bool SocketCan::openDevice(const QString& ifName) {
    if (m_socket >= 0) closeDevice();
    
    m_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (m_socket < 0) return false;

    struct ifreq ifr;
    strncpy(ifr.ifr_name, ifName.toLocal8Bit().constData(), IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    
    if (ioctl(m_socket, SIOCGIFINDEX, &ifr) < 0) {
        closeDevice();
        return false;
    }

    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(m_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        closeDevice();
        return false;
    }
    
    m_ifName = ifName;
    return true;
}

void SocketCan::closeDevice() {
    if (m_socket >= 0) {
        close(m_socket);
        m_socket = -1;
    }
}

bool SocketCan::isOpen() const { return m_socket >= 0; }

bool SocketCan::writeFrame(uint32_t canId, const QByteArray& data) {
    if (!isOpen() || data.size() > CAN_MAX_DLEN) return false;
    
    struct can_frame frame;
    frame.can_id = canId;
    frame.can_dlc = data.size();
    memcpy(frame.data, data.constData(), data.size());

    return write(m_socket, &frame, sizeof(struct can_frame)) == sizeof(struct can_frame);
}

bool SocketCan::readFrame(uint32_t& canId, QByteArray& data) {
    if (!isOpen()) return false;

    struct can_frame frame;
    int nbytes = read(m_socket, &frame, sizeof(struct can_frame));
    if (nbytes < 0) return false;

    canId = frame.can_id;
    data = QByteArray(reinterpret_cast<const char*>(frame.data), frame.can_dlc);
    return true;
}
