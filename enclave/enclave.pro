TEMPLATE = \
	lib

CONFIG += \
	staticlib

QT += \
    network

QT -= \
    gui

TARGET = \
	enclave

unix {
    QMAKE_CXXFLAGS += \
		-std=c++11
}

win32-g++ {
	QMAKE_CXXFLAGS += \
		-std=c++11
}

HEADERS += \
	enclave/network/network_worker.h \
	enclave/client/client.h \
	enclave/server/server.h \
    enclave/server/server_worker.h \
    enclave/client/client_controller.h \
    enclave/client/client_worker.h \
    enclave/network/network_stream.h \
    enclave/server/server_controller.h \
	enclave/network/network_connection.h

SOURCES += \
	enclave/network/network_worker.cpp \
	enclave/client/client.cpp \
	enclave/server/server.cpp \
    enclave/server/server_worker.cpp \
    enclave/client/client_controller.cpp \
    enclave/client/client_worker.cpp \
    enclave/network/network_stream.cpp \
    enclave/server/server_controller.cpp \
	enclave/network/network_connection.cpp
