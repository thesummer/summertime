# Protocol Version 1.0

This document describes the packet protocol for the summertime project for exchanging
data between the IoT measurement device and the mobile user application.

The packets are transmitted between the two devices using UDP which is also responsible
to check the data integrity of payload data. Hence this protocol has no dedicated 
checksum.

In this protocol the mobile user application acts as the master, the IoT device as the
slave. 

UDP is a connectionless protocol. In order to ensure that the destination of a packet
has received it successfully acknowledgement packets are sent back to the packet source.

## Protocol features

In the following MOB will describe the mobile user application and IoT the tick
measurement device.
The protocol has the following general features:

- Check connection (ping): MOB sends a packet to IoT. IoT sends back an acknowledgement
- Start measurement: MOB sends a start packet to IoT. IoT acknowledges this packet and
  starts measuring the ticks and sending measurement packets.
- Stop measurement: MOB sends a stop packet to IoT. IoT acknowledges this packet and
  stops sending measurement packets and stops the measurement.
- Live measurement: IoT sends every 2-5 s containing the current tick frequency.
  MOB acknowledges packet.
- Last minute count: IoT sends every minute a packet containing the number of ticks
  counted during the last minute:
  MOB acknowledges this packet.

Acknowledgements have to be received within 2s. Otherwise the connection is seen as lost.

## Protocol Layout

Each packet starts with a 32 bit packet header followed by application data depending
on its type.
The protocol serialzation is in network byte order i.e. MSB first.

### Packet header

Each packet starts with packet header consisting of unique packet type identifier of 16 bit, followed by a 16 bit sequence count to identify the packet.


+-------------------------------------------------+
|                         |                       |
|      Packet type        | Packet sequence count |
|       (uint16_t)        |      (uint16_t)       |
+-------------------------------------------------+

### Ping packet

- Packet type: 1
- Application data: none

+-------------------------------------------------+
|                         |                       |
|      Packet type (1)    | Packet sequence count |
|       (uint16_t)        |      (uint16_t)       |
+-------------------------------------------------+
### Acknowledgement packet

The acknowledgement packet carries the sequence count of the packet it acknowledges.

- Packet type: 2
- Application data: Sequcence count of previously received packet

+-------------------------------------------------------------------------+
|                         |                       |                       |
|      Packet type (2)    | Packet sequence count |   Ack sequence count  |
|       (uint16_t)        |      (uint16_t)       |      (uint16_t)       |
+-------------------------------------------------------------------------+

## Start measurement packet

Start measurement packets received while a measurement is already in progress
will be ignored.

- Packet type: 3
- Application data: none

+-------------------------------------------------+
|                         |                       |
|      Packet type (3)    | Packet sequence count |
|       (uint16_t)        |      (uint16_t)       |
+-------------------------------------------------+

## Stop measurement packet

Stop measurement packets received while no measurement is in progress
will be ignored.

- Packet type: 4
- Application data: none

+-------------------------------------------------+
|                         |                       |
|      Packet type (3)    | Packet sequence count |
|       (uint16_t)        |      (uint16_t)       |
+-------------------------------------------------+

## Live measurement packet

The current tick frequency will be transmitted as a 32 bit floating point number in Hz.

- Packet type: 5
- Application data: IEEE-754 floating point

+-------------------------------------------------------------------------+
|                         |                       |                       |
|      Packet type (3)    | Packet sequence count |    Tick frequnecy     |
|       (uint16_t)        |      (uint16_t)       |       (float)         |
+-------------------------------------------------------------------------+

## Last minute count packet

The tick count during the last minute will be transmitted as a 16 bit integer.

- Packet type: 5
- Application data: Tick count of the last minute

+-------------------------------------------------------------------------+
|                         |                       |                       |
|      Packet type (3)    | Packet sequence count |   Last minute count   |
|       (uint16_t)        |      (uint16_t)       |       (uint16_t)      |
+-------------------------------------------------------------------------+

