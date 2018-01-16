#ifndef REMOTECONNECTION_H
#define REMOTECONNECTION_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
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
        ActiveMeasurement,
        ErrInvalidAddress
    };
    Q_ENUMS(ConnectionStatus) // Make the enum known to qml

signals:
    void
    statusChanged(ConnectionStatus newStatus);

    void
    updateLiveData(float newRate);

    void
    appendNewMinuteCount(float newCount);

public slots:
    void
    handlePacketTimeout();

    void
    connectSocket(QString ipAddress);

    void
    disconnectSocket();

    void
    handleNewPacket();

    bool
    sendPacket(const protocol::Packet& packet);

    void
    sendHeartbeat();

    void
    startMeasurement();

    void
    stopMeasurement();

private:
    void
    updateTimer();

    struct Timetag
    {
        QDateTime   timestamp;
        quint16 sequenceCount;

        bool operator==(const Timetag& other) const
        {
            return (this->sequenceCount == other.sequenceCount);
        }
    };

    const quint16 mListenPort = 45567;
    static const size_t timeoutMs    = 2000;
    static const size_t hearbeatIntervalMs = 2000;

    QTimer mTimeoutTimer;
    QTimer mHeartbeatTimer;
    QList<Timetag> mPacketQueue;

    ConnectionStatus mStatus = ConnectionStatus::Disconnected;
    QUdpSocket mSocket;
    QHostAddress mRemoteAddress;
    quint16 mRemotePort = 45568;
};

#endif // REMOTECONNECTION_H
