#ifndef REMOTECONNECTION_H
#define REMOTECONNECTION_H

#include <QObject>
#include <QUdpSocket>

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
    connectSocket(QString ipAddress);

    void
    disconnectSocket();

    void
    handleNewPacket();

private:
    const quint16 mListenPort = 45567;

    ConnectionStatus mStatus = ConnectionStatus::Disconnected;
    QUdpSocket mSocket;
    QHostAddress mRemoteAddress;
};

#endif // REMOTECONNECTION_H
