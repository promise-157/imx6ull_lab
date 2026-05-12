#ifndef SOCKETCAN_H
#define SOCKETCAN_H

#include <QString>
#include <QByteArray>
#include <linux/can.h>
#include <linux/can/raw.h>

class SocketCan {
public:
    SocketCan();
    ~SocketCan();

    bool openDevice(const QString& ifName);
    void closeDevice();
    bool isOpen() const;

    bool writeFrame(uint32_t canId, const QByteArray& data);
    bool readFrame(uint32_t& canId, QByteArray& data);

private:
    int m_socket;
    QString m_ifName;
};

#endif // SOCKETCAN_H
