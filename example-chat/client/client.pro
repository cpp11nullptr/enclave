TEMPLATE = \
	app

QT += \
	core \
	network \
	widgets

TARGET = \
	chat-client

unix {
	QMAKE_CXXFLAGS += \
		-std=c++11
}

win32-g++ {
	QMAKE_CXXFLAGS += \
		-std=c++11
}

SOURCES += \
	main.cpp \
	window.cpp

HEADERS += \
	window.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../shared/ -lchat-shared
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../shared/ -lchat-sharedd
else:unix: LIBS += -L$$OUT_PWD/../shared/ -lchat-shared

INCLUDEPATH += $$PWD/../shared
DEPENDPATH += $$PWD/../shared

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../shared/libchat-shared.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../shared/libchat-sharedd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../shared/chat-shared.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../shared/chat-sharedd.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../shared/libchat-shared.a
