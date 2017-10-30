#ifndef REMOTECONNECTION_H
#define REMOTECONNECTION_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QUdpSocket>
#include <QList>

#include <packet.h>

class RemoteConnection : public QObject
{
    Q_OBJECT
public:
    explicit
    RemoteConnection(QObject *parent = nullptr);

    enum class ConnectionStatus
    {
        Disconnected,
        Connected,
        ErrInvalidAddress
    };
    Q_ENUMS(ConnectionStatus) // Make the enum known to qml

signals:
    void statusChanged(ConnectionStatus newStatus);

public slots:
    void
    handlePacketTimeout();

    void
    connectSocket(QString ipAddress);

    void
    disconnectSocket();

    void
    handleNewPacket();

    bool sendPacket(const protocol::Packet& packet);

private:
    void
    updateTimer();

    struct Timetag
    {
        QTime   timestamp;
        quint16 sequenceCount;

        bool operator==(const Timetag& other) const
        {
            return (this->sequenceCount == other.sequenceCount);
        }
    };

    const quint16 mListenPort = 45567;

    QTimer mTimer;
    QList<Timetag> mPacketQueue;

    ConnectionStatus mStatus = ConnectionStatus::Disconnected;
    QUdpSocket mSocket;
    QHostAddress mRemoteAddress;
    quint16 mRemotePort = 45568;
};

#endif // REMOTECONNECTION_H
