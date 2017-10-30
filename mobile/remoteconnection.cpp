#include "remoteconnection.h"
#include <QNetworkDatagram>
#include <iostream>

#include <qdatastreamwriter.h>

RemoteConnection::RemoteConnection(QObject *parent) :
    QObject(parent),
    mTimer(this),
    mSocket(this)
{
    mTimer.setSingleShot(true);
    connect(&mSocket, &QUdpSocket::readyRead, this, &RemoteConnection::handleNewPacket);
    connect(&mTimer, &QTimer::timeout, this, &RemoteConnection::handlePacketTimeout);
}

void
RemoteConnection::handlePacketTimeout()
{
    std::cout << "Warning: Packet timed out" << std::endl;
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
        mPacketQueue.clear();
        mTimer.stop();
        mStatus = ConnectionStatus::Disconnected;
        emit statusChanged(mStatus);
    }
}

void RemoteConnection::handleNewPacket()
{
    QNetworkDatagram newData = mSocket.receiveDatagram();
    QDataStreamReader reader(newData.data());
    protocol::PacketType packetType = static_cast<protocol::PacketType> (reader.readUint16());
    reader.reset();

    switch (packetType)
    {
    case protocol::PacketType::acknowledge:
    {
        protocol::AcknowledgePacket packet;
        if (packet.deserialize(reader))
        {
            // We assume that the sequence count in the list is unique
            mPacketQueue.removeOne(Timetag{QTime(), packet.getAckSequenceCount()});
            updateTimer();
        }
    }

    }
}

bool
RemoteConnection::sendPacket(const protocol::Packet &packet)
{
    QDataStreamWriter buffer(packet.getSerializedSize());
    packet.serialize(buffer);
    qint64 ret = mSocket.writeDatagram(QNetworkDatagram(buffer.getByteArray(),
                                  mRemoteAddress,
                                  mRemotePort));
    if (ret == static_cast<qint64>(packet.getSerializedSize()))
    {
        mPacketQueue.push_front(Timetag{QTime::currentTime(), packet.getSequenceCount()});
        updateTimer();
        return true;
    }
    else
    {
        return false;
    }
}

void
RemoteConnection::updateTimer()
{
    if (!mPacketQueue.isEmpty())
    {
        // Will restart the timer. As the oldest packet is always in last
        // restarting on the same packet is ok.
        mTimer.start(mPacketQueue.last().timestamp.msecsTo(QTime::currentTime()));
    }
}
