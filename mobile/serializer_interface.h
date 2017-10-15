#ifndef SERIALIZER_INTERFACE_H
#define SERIALIZER_INTERFACE_H

#include <stddef.h>
#include <stdint.h>

class SerializerInterface
{
public:
    virtual size_t
    getSize() = 0;

    virtual void
    reset() = 0;

    virtual void
    skip(const size_t bytes) = 0;

    virtual void
    storeFloat(float data) = 0;

    virtual void
    storeUint16(uint16_t data) = 0;
};

class DeserializerInterface
{
public:
    virtual size_t
    getSize() = 0;

    virtual void
    reset() = 0;

    virtual void
    skip(const size_t bytes) = 0;

    virtual float
    readFloat() = 0;

    virtual uint16_t
    readUint16() = 0;
};

#endif // SERIALIZER_INTERFACE_H
