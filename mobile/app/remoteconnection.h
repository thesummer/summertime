#ifndef REMOTECONNECTION_H
#define REMOTECONNECTION_H

#include <QObject>
#include <QBluetoothServiceDiscoveryAgent>
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
        BluetoothDisabled,
        DeviceNotFound,
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
    connectDevice();

    void
    disconnect();

    void
    handleNewPacket();

    void
    startMeasurement();

    void
    stopMeasurement();

private slots:
    void
    newDevice(const QBluetoothDeviceInfo& info);

    void
    deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error);

    void
    deviceScanFinished();

private:
    QBluetoothDeviceDiscoveryAgent mDiscoveryAgent;
    QBluetoothDeviceInfo  mDevice;
    ConnectionStatus mStatus = ConnectionStatus::Disconnected;
};

#endif // REMOTECONNECTION_H
