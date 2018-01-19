#include "packethandler.h"

#include <qdatastreamwriter.h>

PacketHandler::PacketHandler(QObject* parent,
                             quint32 ticksPerMinute,
                             quint32 epsilon,
                             qint16 listenPort) :
    QObject(parent),
    mListenPort(listenPort),
    mTicksPerMinute(ticksPerMinute),
    mEpsilon(epsilon),
    mSocket(this)
{
    // Timeout timer is triggered manually
    mTimeoutTimer.setSingleShot(true);
    connect(&mTimeoutTimer, &QTimer::timeout, this,
            &PacketHandler::handlePacketTimeout);

    // Last minute timer creates a last minute count packet every minute
    mLastMinuteTimer.setSingleShot(false);
    mLastMinuteTimer.setInterval(60000);
    connect(&mLastMinuteTimer, &QTimer::timeout, this,
            &PacketHandler::generateLastMinutePackage);
    // Live packet timer will send a packet every 10 ticks or 5 s
    mLastMinuteTimer.setSingleShot(false);
    if (mTicksPerMinute < 120)
    {
        mLivePackageTimer.setInterval(5000);
    }
    else
    {
        mLivePackageTimer.setInterval(60*1000*10/mTicksPerMinute);
    }
    qDebug() << "Live Package interval  " << mLivePackageTimer.interval() << " msec";
    connect(&mLivePackageTimer, &QTimer::timeout, this,
            &PacketHandler::generateLivePackage);

    connect(&mSocket, &QUdpSocket::readyRead, this,
            &PacketHandler::handleNewPacket);
}

void
PacketHandler::connectSocket()
{
    if (mStatus != ConnectionStatus::Connected)
    {
        if (mSocket.bind(mListenPort))
        {
            qDebug("Changed status to Connected");
            mStatus = ConnectionStatus::Connected;
        }
        else
        {
            mStatus = ConnectionStatus::ErrBindFailed;
        }
    }
}

void
PacketHandler::disconnectSocket()
{
    if (mStatus == ConnectionStatus::Connected || mStatus == ConnectionStatus::ActiveMeasurement)
    {
        mSocket.close();
        mPacketQueue.clear();
        mStatus = ConnectionStatus::Disconnected;
        qDebug("Changed status to Disconnected");
    }
}

void
PacketHandler::handleNewPacket()
{
    QNetworkDatagram newData = mSocket.receiveDatagram();
    QDataStreamReader reader(newData.data());
    // Not very elegant:
    mRemoteAddress = newData.senderAddress();
    mRemotePort    = newData.senderPort();

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
        break;
    }
    case protocol::PacketType::ping:
    {
        qDebug("Received heartbeat request");
        protocol::PingPacket packet;
        if (packet.deserialize(reader))
        {
            sendPacket(protocol::AcknowledgePacket(packet));
        }
        break;
    }
    case protocol::PacketType::startMeasurement:
     {
        qDebug("Received start measurement request");
        mLastMinuteTimer.start();
        mLivePackageTimer.start();
        protocol::StartMeasurementPacket packet;
        if (packet.deserialize(reader))
        {
            sendPacket(protocol::AcknowledgePacket(packet));
        }
        mStatus = ConnectionStatus::ActiveMeasurement;
        break;
    }
    case protocol::PacketType::stopMeasurement:
    {
        qDebug("Received stop measurement request");
        mLastMinuteTimer.stop();
        mLivePackageTimer.stop();
        protocol::StopMeasurementPacket packet;
        if (packet.deserialize(reader))
        {
            sendPacket(protocol::AcknowledgePacket(packet));
        }
        mStatus = ConnectionStatus::Connected;
        break;
    }
    default:
        break;
    }
}

void
PacketHandler::handlePacketTimeout()
{
    qWarning("Warning: Packet timed out");
    switch (mStatus)
    {
    case ConnectionStatus::ActiveMeasurement:
        mLastMinuteTimer.stop();
        mLivePackageTimer.stop();
    case ConnectionStatus::Connected:
        disconnectSocket();
        break;
    default:
        break;
    }
}

bool
PacketHandler::sendPacket(const protocol::Packet& packet)
{
    QDataStreamWriter buffer(packet.getSerializedSize());
    qDebug("Type: %d", packet.getPacketType());
    packet.serialize(buffer);

    qint64 ret = mSocket.writeDatagram(QNetworkDatagram(buffer.getByteArray(),
                                  mRemoteAddress,
                                  mRemotePort));
    if (ret == static_cast<qint64>(packet.getSerializedSize()))
    {
        if (packet.getPacketType() != protocol::PacketType::acknowledge)
        {
            mPacketQueue.push_front(Timetag{QDateTime::currentDateTime(), packet.getSequenceCount()});
            updateTimer();
        }
        return true;
    }
    else
    {
        return false;
    }
}

void
PacketHandler::generateLastMinutePackage()
{
    quint32 high = mTicksPerMinute + mEpsilon;
    quint32 low  = mTicksPerMinute - mEpsilon;
    protocol::LastMinuteCountPacket packet;
    packet.setLastMinuteCount(qrand() % ((high + 1) - low) + low);
    sendPacket(packet);
}

void
PacketHandler::generateLivePackage()
{
    quint32 high = mTicksPerMinute + mEpsilon;
    quint32 low  = mTicksPerMinute - mEpsilon;
    quint32 ticksPerMinute = qrand() % ((high + 1) - low) + low;
    protocol::LiveMeasurementPacket packet;

    packet.setTickFrequency(ticksPerMinute/60.0);
    sendPacket(packet);
}

void
PacketHandler::updateTimer()
{
    if (!mPacketQueue.isEmpty())
    {
        // Will restart the timer. As the oldest packet is always in last
        // restarting on the same packet is ok.
        auto timeout = QDateTime::currentDateTime().msecsTo(mPacketQueue.last().timestamp.addMSecs(timeoutMs));
        if (timeout > 0)
        {
            mTimeoutTimer.start(timeout);
        }
        else
        {
            mPacketQueue.pop_back();
            handlePacketTimeout();
        }
    }
    else
    {
        mTimeoutTimer.stop();
    }
}
