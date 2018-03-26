#ifndef REMOTECONNECTION_H
#define REMOTECONNECTION_H

#include <QObject>
#include <QBluetoothServiceDiscoveryAgent>
#include <QLowEnergyController>
#include <QList>
#include <QContiguousCache>

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
    newService(const QBluetoothUuid &newServiceUuid);

    void
    serviceScanFinished();

    void
    serviceDetailsDiscovered(QLowEnergyService::ServiceState newState);

    void
    handleChangedCharacteristic(const QLowEnergyCharacteristic &characteristic,
                                const QByteArray &newValue);

private:
    const size_t CacheSize = 10;
    const size_t ClockFreq = 32768; ///< The clock frequency of the low freq clock of the nrf
    QBluetoothDeviceDiscoveryAgent mDiscoveryAgent;
    QLowEnergyController* mController = nullptr;
    QBluetoothDeviceInfo  mDevice;
    const QBluetoothUuid mServiceUuid = QBluetoothUuid(QString("{0000f00d-1212-efde-1523-785fef13d123}"));
    const QBluetoothUuid mLiveDataUuid = QBluetoothUuid(QString("00000a0a-1212-efde-1523-785fef13d123"));
    const QBluetoothUuid mRateDataUuid = QBluetoothUuid(QString("00000b0b-1212-efde-1523-785fef13d123"));
    QLowEnergyCharacteristic mLiveData;
    QLowEnergyDescriptor mLiveDataCccd;
    QLowEnergyCharacteristic mRateData;
    QLowEnergyDescriptor mRateDataCccd;
    QLowEnergyService* mService = nullptr;
    ConnectionStatus mStatus = ConnectionStatus::Disconnected;
    QContiguousCache<double> mCache;
    quint64 mLastTick;
    double mCurrentRatePerSec;
};

#endif // REMOTECONNECTION_H
