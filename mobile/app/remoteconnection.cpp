#include "remoteconnection.h"
#include <iostream>

RemoteConnection::RemoteConnection(QObject *parent) :
    QObject(parent)
{
}

void
RemoteConnection::connect()
{
    if (mStatus != ConnectionStatus::Connected)
    {
        // TODO: Implement
        emit statusChanged(mStatus);
    }
}

void
RemoteConnection::disconnect()
{
    if (mStatus == ConnectionStatus::Connected || mStatus == ConnectionStatus::ActiveMeasurement)
    {
        qDebug("Changed status to Disconnected");
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
