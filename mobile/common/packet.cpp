#include "packet.h"

using namespace protocol;

uint16_t Packet::globalSequenceCount = 0;

bool
PingPacket::serialize(SerializerInterface &writer) const
{
    if (writer.getSize() < getSerializedSize())
    {
        return false;
    }
    else
    {
        writer.storeUint16(static_cast<uint16_t>(getPacketType()));
        writer.storeUint16(mSequenceCount);
        return true;
    }
}

bool
PingPacket::deserialize(DeserializerInterface &reader)
{
    if (reader.getSize() < getSerializedSize() && reader.readUint16() != static_cast<uint16_t>(getPacketType()))
    {
        return false;
    }
    else
    {
        mSequenceCount = reader.readUint16();
        return true;
    }
}

uint16_t
AcknowledgePacket::getAckSequenceCount() const
{
    return mAckSequenceCount;
}

bool
AcknowledgePacket::serialize(SerializerInterface &writer) const
{
    if (writer.getSize() < getSerializedSize())
    {
        return false;
    }
    else
    {
        writer.storeUint16(static_cast<uint16_t>(getPacketType()));
        writer.storeUint16(mSequenceCount);
        writer.storeUint16(mAckSequenceCount);
        return true;
    }
}

bool
AcknowledgePacket::deserialize(DeserializerInterface &reader)
{
    if (reader.getSize() < getSerializedSize() && reader.readUint16() != static_cast<uint16_t>(getPacketType()))
    {
        return false;
    }
    else
    {
        mSequenceCount    = reader.readUint16();
        mAckSequenceCount = reader.readUint16();
        return true;
    }
}

bool
StartMeasurementPacket::serialize(SerializerInterface &writer) const
{
    if (writer.getSize() < getSerializedSize())
    {
        return false;
    }
    else
    {
        writer.storeUint16(static_cast<uint16_t>(getPacketType()));
        writer.storeUint16(mSequenceCount);
        return true;
    }
}

bool
StartMeasurementPacket::deserialize(DeserializerInterface &reader)
{
    if (reader.getSize() < getSerializedSize() && reader.readUint16() != static_cast<uint16_t>(getPacketType()))
    {
        return false;
    }
    else
    {
        mSequenceCount = reader.readUint16();
        return true;
    }
}

bool
StopMeasurementPacket::serialize(SerializerInterface &writer) const
{
    if (writer.getSize() < getSerializedSize())
    {
        return false;
    }
    else
    {
        writer.storeUint16(static_cast<uint16_t>(getPacketType()));
        writer.storeUint16(mSequenceCount);
        return true;
    }
}

bool
StopMeasurementPacket::deserialize(DeserializerInterface &reader)
{
    if (reader.getSize() < getSerializedSize() && reader.readUint16() != static_cast<uint16_t>(getPacketType()))
    {
        return false;
    }
    else
    {
        mSequenceCount = reader.readUint16();
        return true;
    }
}

bool
LiveMeasurementPacket::serialize(SerializerInterface &writer) const
{
    if (writer.getSize() < getSerializedSize())
    {
        return false;
    }
    else
    {
        writer.storeUint16(static_cast<uint16_t>(getPacketType()));
        writer.storeUint16(mSequenceCount);
        writer.storeFloat(mTickFrequency);
        return true;
    }
}

bool
LiveMeasurementPacket::deserialize(DeserializerInterface &reader)
{
    if (reader.getSize() < getSerializedSize() && reader.readUint16() != static_cast<uint16_t>(getPacketType()))
    {
        return false;
    }
    else
    {
        mSequenceCount = reader.readUint16();
        mTickFrequency = reader.readFloat();
        return true;
    }
}

bool
LastMinuteCountPacket::serialize(SerializerInterface &writer) const
{
    if (writer.getSize() < getSerializedSize())
    {
        return false;
    }
    else
    {
        writer.storeUint16(static_cast<uint16_t>(getPacketType()));
        writer.storeUint16(mSequenceCount);
        writer.storeUint16(mLastMinuteCount);
        return true;
    }
}

bool
LastMinuteCountPacket::deserialize(DeserializerInterface &reader)
{
    if (reader.getSize() < getSerializedSize() && reader.readUint16() != static_cast<uint16_t>(getPacketType()))
    {
        return false;
    }
    else
    {
        mSequenceCount   = reader.readUint16();
        mLastMinuteCount = reader.readUint16();
        return true;
    }
}
