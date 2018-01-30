TEMPLATE = subdirs

CONFIG += c++11

SUBDIRS = common app emulator

mobile.depends = common
emulator.depends = common

SOURCES += \
    ../examples/outpost-minimal/ble_thread.cpp
