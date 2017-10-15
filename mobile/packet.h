#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <serializer_interface.h>

namespace protocol
{

enum class PacketType
{
    ping             = 1,
    acknowledge      = 2,
    startMeasurement = 3,
    stopMeasurement  = 4,
    liveMeasurement  = 5,
    lastMinuteCount  = 6
};

/**
 * Base class for all packet
 *
 * Will make sure the sequence count will be
 * incremented with each packet instance
 */
class Packet
{    
public:
    Packet(PacketType packetType) :
        mSequenceCount(++globalSequenceCount),
        mPacketType(packetType)
    {
    }

    virtual PacketType
    getPacketType() const
    {
        return mPacketType;
    }

    uint16_t
    getSequenceCount() const
    {
        return mSequenceCount;
    }

    virtual bool
    serialize(SerializerInterface& writer) = 0;

    virtual bool
    deserialize(DeserializerInterface& reader) = 0;

protected:
    static void
    resetGlobalSequenceCount()
    {
        globalSequenceCount = 0;
    }

    uint16_t mSequenceCount;
    PacketType mPacketType;

private:
    static uint16_t globalSequenceCount;
};

class PingPacket : public Packet
{
public:
    const size_t packetSize = 4;

    PingPacket() :
        Packet(PacketType::ping)
    {
    }

    virtual bool
    serialize(SerializerInterface& writer);

    virtual bool
    deserialize(DeserializerInterface& reader);
};

class AcknowledgePacket : public Packet
{
public:
    const size_t packetSize = 6;

    AcknowledgePacket() :
        Packet(PacketType::acknowledge),
        mAckSequenceCount(0)
    {
    }

    AcknowledgePacket(Packet& packet) :
        Packet(PacketType::acknowledge),
        mAckSequenceCount(packet.getSequenceCount())
    {
    }

    AcknowledgePacket(uint16_t ackSequenceCout) :
        Packet(PacketType::acknowledge),
        mAckSequenceCount(ackSequenceCout)
    {
    }

    virtual bool
    serialize(SerializerInterface& writer);

    virtual bool
    deserialize(DeserializerInterface& reader);

private:
    uint16_t mAckSequenceCount;
};

class StartMeasurementPacket : public Packet
{
public:
    const size_t packetSize = 4;

    StartMeasurementPacket() :
        Packet(PacketType::startMeasurement)
    {
    }

    virtual bool
    serialize(SerializerInterface& writer);

    virtual bool
    deserialize(DeserializerInterface& reader);
};

class StopMeasurementPacket : public Packet
{
public:
    const size_t packetSize = 4;

    StopMeasurementPacket() :
        Packet(PacketType::stopMeasurement)
    {
    }

    virtual bool
    serialize(SerializerInterface& writer);

    virtual bool
    deserialize(DeserializerInterface& reader);
};

class LiveMeasurementPacket : public Packet
{
public:
    const size_t packetSize = 8;

    LiveMeasurementPacket() :
        Packet(PacketType::liveMeasurement),
        mTickFrequency(0.0f)
    {
    }

    float
    getTickFrequency() const
    {
        return mTickFrequency;
    }

    void
    setTickFrequency(float tickFrequency)
    {
        mTickFrequency = tickFrequency;
    }

    virtual bool
    serialize(SerializerInterface& writer);

    virtual bool
    deserialize(DeserializerInterface& reader);

private:
    float mTickFrequency;
};

class LastMinuteCountPacket : public Packet
{
public:
    const size_t packetSize = 6;

    LastMinuteCountPacket() :
        Packet(PacketType::lastMinuteCount)
    {
    }

    uint16_t
    getLastMinuteCount() const
    {
        return mLastMinuteCount;
    }

    void setLastMinuteCount(const uint16_t &lastMinuteCount)
    {
        mLastMinuteCount = lastMinuteCount;
    }

    virtual bool
    serialize(SerializerInterface& writer);

    virtual bool
    deserialize(DeserializerInterface& reader);

private:
    uint16_t mLastMinuteCount;
};


} // namespace protocol

#endif //
