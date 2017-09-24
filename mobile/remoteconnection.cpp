#include "remoteconnection.h"
#include <QNetworkDatagram>

RemoteConnection::RemoteConnection(QObject *parent) :
    QObject(parent),
    mSocket(this)
{
    connect(&mSocket, &QUdpSocket::readyRead, this, &RemoteConnection::handleNewPacket);
}

void
RemoteConnection::connectSocket(QString ipAddress)
{
    if (mStatus != ConnectionStatus::Connected)
    {
        if (mRemoteAddress.setAddress(ipAddress) && mSocket.bind(mListenPort))
        {
            mStatus = ConnectionStatus::Connected;
        }
        else
        {
            mStatus = ConnectionStatus::ErrInvalidAddress;
        }
        emit statusChanged(mStatus);
    }
}

void
RemoteConnection::disconnectSocket()
{
    if (mStatus == ConnectionStatus::Connected)
    {
        mSocket.close();
        mStatus = ConnectionStatus::Disconnected;
        emit statusChanged(mStatus);
    }
}

void RemoteConnection::handleNewPacket()
{
    QNetworkDatagram newData = mSocket.receiveDatagram();
    mSocket.writeDatagram(newData.makeReply(newData.data()));
}
