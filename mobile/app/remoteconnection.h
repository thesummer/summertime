#ifndef REMOTECONNECTION_H
#define REMOTECONNECTION_H

#include <QObject>

#include <QList>

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
    connect();

    void
    disconnect();

    void
    handleNewPacket();

    void
    startMeasurement();

    void
    stopMeasurement();

private:

    ConnectionStatus mStatus = ConnectionStatus::Disconnected;
};

#endif // REMOTECONNECTION_H
