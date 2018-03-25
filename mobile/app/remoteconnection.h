#ifndef REMOTECONNECTION_H
#define REMOTECONNECTION_H

#include <QObject>
#include <QBluetoothServiceDiscoveryAgent>
#include <QLowEnergyController>
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
        Scanning,
        DeviceNotFound,
        Connecting,
        Connected,
        ConnectError,
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

    void
    deviceConnected();

    void
    deviceConnectError(QLowEnergyController::Error newError);

    void
    deviceDisconnected();

    void
    newService(const QBluetoothUuid &newService);

    void
    serviceScanFinished();

private:
    QBluetoothDeviceDiscoveryAgent mDiscoveryAgent;
    QLowEnergyController* mController = nullptr;
    QBluetoothDeviceInfo  mDevice;
    ConnectionStatus mStatus = ConnectionStatus::Disconnected;
};

#endif // REMOTECONNECTION_H
