#ifndef QDATASTREAMWRITER_H
#define QDATASTREAMWRITER_H

#include <QByteArray>
#include <QDataStream>
#include <QNetworkDatagram>

#include <serializer_interface.h>

class QDataStreamWriter : public SerializerInterface
{
public:
    QDataStreamWriter(size_t size);

    // SerializerInterface interface
    size_t
    getSize();

    void
    reset();

    void
    skip(const size_t bytes);

    void
    storeFloat(float data);

    void
    storeUint16(uint16_t data);

private:
    QByteArray mDataBuffer;
    QDataStream mDataWriter;
};

class QDataStreamReader : public DeserializerInterface
{
public:
    QDataStreamReader(const QByteArray data);

    QDataStreamReader(const QNetworkDatagram data);

    size_t
    getSize();

    void
    reset();

    void
    skip(const size_t bytes);

    float
    readFloat();

    uint16_t
    readUint16();

private:
    QByteArray mDataBuffer;
    QDataStream mDataReader;
    size_t mSize;
};


#endif // QDATASTREAMWRITER_H
