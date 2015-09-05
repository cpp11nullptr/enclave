TEMPLATE = \
	lib

CONFIG += \
	staticlib

QT += \
	network

QT -= \
	gui

TARGET = \
	chat-shared

HEADERS += \
	shared/command.h \
    shared/helper.h

SOURCES += \
    shared/helper.cpp
