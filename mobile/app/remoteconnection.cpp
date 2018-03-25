#include "remoteconnection.h"
#include <iostream>

RemoteConnection::RemoteConnection(QObject *parent) :
    QObject(parent),
    mDevice()
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
    }
}

void
RemoteConnection::stopMeasurement()
{
    if (mService)
    {
        mService->writeDescriptor(mLiveDataCccd, QByteArray::fromHex("0000"));
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
    mStatus = ConnectionStatus::Connected;
    emit statusChanged(mStatus);
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

void RemoteConnection::handleChangedCharacteristic(const QLowEnergyCharacteristic &characteristic,
                                                   const QByteArray &/*newValue*/)
{
    if (characteristic == mLiveData)
    {
        qDebug("New Live data package");
    }
    else if (characteristic == mRateData)
    {
        qDebug("New Rate data package");
    }

}
