CONFIG += \
	console

CONFIG -= \
	app_bundle

QT += \
	core \
	network

QT -= \
	gui

TARGET = \
	benchmark-server

unix {
	QMAKE_CXXFLAGS += \
		-std=c++11
}

win32-g++ {
	QMAKE_CXXFLAGS += \
		-std=c++11
}

HEADERS += \
    server.h

SOURCES += \
    server.cpp \
    main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../enclave/ -lenclave
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../enclave/ -lenclaved
else:unix: LIBS += -L$$OUT_PWD/../../enclave/ -lenclave

INCLUDEPATH += $$PWD/../../enclave
DEPENDPATH += $$PWD/../../enclave

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../enclave/libenclave.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../enclave/libenclaved.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../enclave/enclave.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../enclave/enclaved.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../enclave/libenclave.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../shared/ -lbenchmark-shared
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../shared/ -lbenchmark-sharedd
else:unix: LIBS += -L$$OUT_PWD/../shared/ -lbenchmark-shared

INCLUDEPATH += $$PWD/../shared
DEPENDPATH += $$PWD/../shared

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../shared/libbenchmark-shared.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../shared/libbenchmark-sharedd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../shared/benchmark-shared.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../shared/benchmark-sharedd.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../shared/libbenchmark-shared.a
