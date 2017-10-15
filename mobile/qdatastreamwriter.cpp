#include "qdatastreamwriter.h"

QDataStreamWriter::QDataStreamWriter(size_t size)
    : mDataBuffer(size, 0),
      mDataWriter(&mDataBuffer, QIODevice::ReadWrite)
{
    mDataWriter.setByteOrder(QDataStream::BigEndian);
}

size_t
QDataStreamWriter::getSize()
{
    return mDataBuffer.size();
}

void
QDataStreamWriter::reset()
{
    mDataWriter.device()->seek(0);
}

void
QDataStreamWriter::skip(const size_t bytes)
{
    mDataWriter.device()->seek(mDataWriter.device()->pos()+bytes);
}

void
QDataStreamWriter::storeFloat(float data)
{
    mDataWriter.setFloatingPointPrecision(QDataStream::SinglePrecision);
    mDataWriter << data;
}

void
QDataStreamWriter::storeUint16(uint16_t data)
{
    mDataWriter << static_cast<quint16>(data);
}


QDataStreamReader::QDataStreamReader(const QByteArray data)
    : mDataReader(data),
      mSize(data.size())
{
    mDataReader.setByteOrder(QDataStream::BigEndian);
}

QDataStreamReader::QDataStreamReader(const QNetworkDatagram data)
    : mDataBuffer(data.data()),
      mSize(data.data().size())
{
    mDataReader.setByteOrder(QDataStream::BigEndian);
}

size_t
QDataStreamReader::getSize()
{
    return mSize;
}

void
QDataStreamReader::reset()
{
    mDataReader.device()->seek(0);
}

void
QDataStreamReader::skip(const size_t bytes)
{
    mDataReader.device()->seek(mDataReader.device()->pos()+bytes);
}

float
QDataStreamReader::readFloat()
{
    float val;
    mDataReader.setFloatingPointPrecision(QDataStream::SinglePrecision);
    mDataReader >> val;
    return val;
}

uint16_t
QDataStreamReader::readUint16()
{
    quint16 val;
    mDataReader >> val;
    return val;
}

