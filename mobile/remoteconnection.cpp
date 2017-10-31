#include "remoteconnection.h"
#include <QNetworkDatagram>
#include <iostream>

#include <qdatastreamwriter.h>

RemoteConnection::RemoteConnection(QObject *parent) :
    QObject(parent),
    mTimeoutTimer(this),
    mSocket(this)
{
    // Timeout timer is triggered manually
    mTimeoutTimer.setSingleShot(true);
    connect(&mTimeoutTimer, &QTimer::timeout, this, &RemoteConnection::handlePacketTimeout);
    // Heartbeat timer is periodical
    mHeartbeatTimer.setSingleShot(false);
    mHeartbeatTimer.setInterval(hearbeatIntervalMs);
    connect(&mHeartbeatTimer, &QTimer::timeout, this, &RemoteConnection::sendHeartbeat);

    connect(&mSocket, &QUdpSocket::readyRead, this, &RemoteConnection::handleNewPacket);
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
            mHeartbeatTimer.start();
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
        mTimeoutTimer.stop();
        mHeartbeatTimer.stop();
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
            mPacketQueue.removeOne(Timetag{QDateTime(), packet.getAckSequenceCount()});
            updateTimer();
        }
    }
    case protocol::PacketType::ping:
    {
        protocol::PingPacket packet;
        if (packet.deserialize(reader))
        {
            protocol::AcknowledgePacket ack(packet);
            sendPacket(ack);
        }
    }

    }
}

bool
RemoteConnection::sendPacket(const protocol::Packet& packet)
{
    QDataStreamWriter buffer(packet.getSerializedSize());
    packet.serialize(buffer);
    qint64 ret = mSocket.writeDatagram(QNetworkDatagram(buffer.getByteArray(),
                                  mRemoteAddress,
                                  mRemotePort));
    if (ret == static_cast<qint64>(packet.getSerializedSize()))
    {
        mPacketQueue.push_front(Timetag{QDateTime::currentDateTime(), packet.getSequenceCount()});
        updateTimer();
        return true;
    }
    else
    {
        return false;
    }
}

void
RemoteConnection::sendHeartbeat()
{
    protocol::PingPacket heartbeatRequest;
    sendPacket(heartbeatRequest);
}

void
RemoteConnection::updateTimer()
{
    if (!mPacketQueue.isEmpty())
    {
        // Will restart the timer. As the oldest packet is always in last
        // restarting on the same packet is ok.
        mTimeoutTimer.start(mPacketQueue.last().timestamp.msecsTo(QDateTime::currentDateTime().addMSecs(timeoutMs)));
    }
}
