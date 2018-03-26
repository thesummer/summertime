#include "remoteconnection.h"
#include <iostream>

RemoteConnection::RemoteConnection(QObject *parent) :
    QObject(parent),
    mDevice(),
    mCache(CacheSize),
    mLastTick(0)
{
    connect(&mDiscoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
              this, SLOT(newDevice(const QBluetoothDeviceInfo&)));
    connect(&mDiscoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
              this, SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
    connect(&mDiscoveryAgent, SIGNAL(finished()), this, SLOT(deviceScanFinished()));
}

void
RemoteConnection::connectDevice()
{
    if (mStatus    != ConnectionStatus::Disconnected || mStatus != ConnectionStatus::BluetoothDisabled
        || mStatus != ConnectionStatus::ConnectError || mStatus != ConnectionStatus::DeviceNotFound)
    {
        mStatus = ConnectionStatus::Scanning;
        emit statusChanged(mStatus);
        mDiscoveryAgent.start();
    }
}

void
RemoteConnection::disconnect()
{
    if (mStatus == ConnectionStatus::Connected || mStatus == ConnectionStatus::ActiveMeasurement)
    {
        if (mController)
        {
            mController->disconnectFromDevice();
        }
        // TODO: Implement
        qDebug("Changed status to Disconnected");
        mStatus = ConnectionStatus::Disconnected;
        emit statusChanged(mStatus);
    }
}

void
RemoteConnection::startMeasurement()
{
    if (mService)
    {
        mService->writeDescriptor(mLiveDataCccd, QByteArray::fromHex("0100"));
        mService->writeDescriptor(mRateDataCccd, QByteArray::fromHex("0100"));
        mStatus = ConnectionStatus::ActiveMeasurement;
        emit statusChanged(mStatus);
    }
}

void
RemoteConnection::stopMeasurement()
{
    if (mService)
    {
        mService->writeDescriptor(mLiveDataCccd, QByteArray::fromHex("0000"));
        mService->writeDescriptor(mRateDataCccd, QByteArray::fromHex("0000"));
        mStatus = ConnectionStatus::Connected;
        emit statusChanged(mStatus);
        mCurrentRatePerSec = 0.0;
        mLastTick = 0;
    }
}

void
RemoteConnection::newDevice(const QBluetoothDeviceInfo& info)
{
    if ((info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
        && (info.name() == "Sommerzeit"))
    {
         mDevice = info;
         qDebug("Found device %s", mDevice.name().toStdString().c_str());
         mDiscoveryAgent.stop();
         if (!mController)
         {
             mController = QLowEnergyController::createCentral(mDevice, this);
             connect(mController, &QLowEnergyController::connected, this, &RemoteConnection::deviceConnected);
             connect(mController, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
                     this, &RemoteConnection::deviceConnectError);
             connect(mController, &QLowEnergyController::disconnected, this, &RemoteConnection::deviceDisconnected);
             connect(mController, &QLowEnergyController::serviceDiscovered, this, &RemoteConnection::newService);
             connect(mController, &QLowEnergyController::discoveryFinished, this, &RemoteConnection::serviceScanFinished);
             mController->setRemoteAddressType(QLowEnergyController::RandomAddress);
         }
         mStatus = ConnectionStatus::Connecting;
         emit statusChanged(mStatus);
         mController->connectToDevice();
     }
}

void
RemoteConnection::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
    {
        mStatus = ConnectionStatus::BluetoothDisabled;
        emit statusChanged(mStatus);
    }
//    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
//        setUpdate("Writing or reading from the device resulted in an error.");
//    else
//        setUpdate("An unknown error has occurred.");

//    m_deviceScanState = false;
//    emit devicesUpdated();
//    emit stateChanged();
}

void
RemoteConnection::deviceScanFinished()
{
    if (!mDevice.isValid())
    {
        mStatus = ConnectionStatus::DeviceNotFound;
    }
    emit statusChanged(mStatus);
}

void
RemoteConnection::deviceConnected()
{
    mController->discoverServices();
}

void
RemoteConnection::deviceConnectError(QLowEnergyController::Error /*newError*/)
{
    mStatus = ConnectionStatus::ConnectError;
    emit statusChanged(mStatus);
}

void
RemoteConnection::deviceDisconnected()
{
    mStatus = ConnectionStatus::Disconnected;
    emit statusChanged(mStatus);
}

void
RemoteConnection::newService(const QBluetoothUuid& newServiceUuid)
{
    if (newServiceUuid == mServiceUuid)
    {
        mService = mController->createServiceObject(newServiceUuid);
        qDebug("Found Service uuid: %s\n", newServiceUuid.toString().toLocal8Bit().data());
        if (!mService)
        {
            qWarning("Cannot create service for uuid");
        }
    }
}

void
RemoteConnection::serviceScanFinished()
{
    if (mService)
    {
        connect(mService, &QLowEnergyService::stateChanged, this, &RemoteConnection::serviceDetailsDiscovered);
        connect(mService, &QLowEnergyService::characteristicChanged, this, &RemoteConnection::handleChangedCharacteristic);
        mService->discoverDetails();
    }
}

void
RemoteConnection::serviceDetailsDiscovered(QLowEnergyService::ServiceState newState)
{
    qDebug("state changed %u", newState);
    if (newState == QLowEnergyService::ServiceDiscovered)
    {
        qDebug("Service discovery finished");
        qDebug("Found %u characteristics", mService->characteristics().size());
        mLiveData = mService->characteristic(mLiveDataUuid);
        mRateData = mService->characteristic(mRateDataUuid);
        if (mLiveData.isValid() && mRateData.isValid())
        {

            mLiveDataCccd = mLiveData.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            mRateDataCccd = mRateData.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            if (mLiveDataCccd.isValid() && mRateDataCccd.isValid())
            {
                qDebug("Found valid descriptos");
                mStatus = ConnectionStatus::Connected;
                emit statusChanged(mStatus);
            }

        }

    }
}

void RemoteConnection::handleChangedCharacteristic(const QLowEnergyCharacteristic& characteristic,
                                                   const QByteArray& newValue)
{
    QDataStream data(newValue);
    data.setByteOrder(QDataStream::BigEndian);
    if (characteristic == mLiveData)
    {
        quint16 minute;
        quint32 counter;
        data >> minute >> counter;
        qDebug("New Live data package: %u min %u", minute, counter);

        if (mLastTick != 0)
        {
            quint64 currentTick = minute*60*ClockFreq + counter;
            double ratePerSec = 0.5 * ClockFreq / (currentTick - mLastTick);
            qDebug("Rate %f", ratePerSec);
            if (mCache.isFull())
            {
                // We have CacheSize values stored. Pop the oldest, update the rate
                // and push the new value to the cache
                double oldRate = mCache.takeFirst();
                mCurrentRatePerSec -= oldRate/CacheSize;
                mCurrentRatePerSec += ratePerSec/CacheSize;
                mCache.append(ratePerSec);
            }
            else
            {
                mCurrentRatePerSec *= mCache.size();
                mCache.append(ratePerSec);
                mCurrentRatePerSec = (mCurrentRatePerSec + ratePerSec) / mCache.size();
            }
            emit updateLiveData(static_cast<float>(mCurrentRatePerSec));
        }
        mLastTick = minute*60*ClockFreq + counter;
    }
    else if (characteristic == mRateData)
    {
        quint16 minute;
        quint16 rate;

        data >> minute >> rate;

        appendNewMinuteCount(static_cast<float>(rate));

        qDebug("New Rate data package: %u min %u", minute, rate);
    }

}
