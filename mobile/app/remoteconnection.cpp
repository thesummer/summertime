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
    if (mStatus != ConnectionStatus::Connected)
    {
        mDiscoveryAgent.start();
    }
}

void
RemoteConnection::disconnect()
{
    if (mStatus == ConnectionStatus::Connected || mStatus == ConnectionStatus::ActiveMeasurement)
    {
        // TODO: Implement
        qDebug("Changed status to Disconnected");
        mStatus = ConnectionStatus::Disconnected;
        emit statusChanged(mStatus);
    }
}

void RemoteConnection::handleNewPacket()
{
}

void
RemoteConnection::startMeasurement()
{
    // TODO: Implement
}

void
RemoteConnection::stopMeasurement()
{
    // TODO: Implement
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
         mStatus = ConnectionStatus::Connected;
         emit statusChanged(mStatus);
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
    if (mDevice.isValid())
    {
        mStatus = ConnectionStatus::Connected;
    }
    else
    {
        mStatus = ConnectionStatus::DeviceNotFound;
    }
    emit statusChanged(mStatus);
}
