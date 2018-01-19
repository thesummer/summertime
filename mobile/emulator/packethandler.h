#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include <QTimer>
#include <QDateTime>
#include <QUdpSocket>
#include <QList>

#include <packet.h>

class PacketHandler : public QObject
{
    Q_OBJECT
public:
    enum class ConnectionStatus
    {
        Disconnected,
        Connected,
        ActiveMeasurement,
        ErrBindFailed
    };
    Q_ENUMS(ConnectionStatus) // Make the enum known to qml

    explicit
    PacketHandler(QObject* parent,
                  quint32 ticksPerMinute,
                  quint32 epsilon = 0,
                  qint16 listenPort = 45568);

public slots:
    void
    connectSocket();

    void
    disconnectSocket();

    void
    handleNewPacket();

    void
    handlePacketTimeout();

    bool
    sendPacket(const protocol::Packet& packet);

private slots:
    void
    generateLastMinutePackage();

    void
    generateLivePackage();

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

    quint16 mListenPort;
    quint32 mTicksPerMinute;
    quint32 mEpsilon;
    static const size_t timeoutMs    = 2000;
    QTimer mTimeoutTimer;
    QTimer mLivePackageTimer;
    QTimer mLastMinuteTimer;
    QList<Timetag> mPacketQueue;

    ConnectionStatus mStatus = ConnectionStatus::Disconnected;
    QUdpSocket mSocket;
    QHostAddress mRemoteAddress;
    quint16 mRemotePort = 45568;

};

#endif // PACKETHANDLER_H
