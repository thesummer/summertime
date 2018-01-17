TEMPLATE = lib
CONFIG += staticlib

QT += network

SOURCES = packet.cpp qdatastreamwriter.cpp
HEADERS = packet.h qdatastreamwriter.h serializer_interface.h
