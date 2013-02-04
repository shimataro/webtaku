QT += core webkit network

TEMPLATE = app
TARGET   = wiget

CONFIG += console warn_on
CONFIG -= app_bundle

QMAKE_CFLAGS   += -ansi
QMAKE_CXXFLAGS += -ansi
#QMAKE_CXXFLAGS += -std=c++0x

CONFIG(debug, debug|release) {
	QMAKE_CFLAGS   += -g -O0
	QMAKE_CXXFLAGS += -g -O0
}
CONFIG(release, debug|release) {
	QMAKE_CFLAGS   += -Ofast
	QMAKE_CXXFLAGS += -Ofast
}


SOURCES += \
	main.cpp \
	snapshot.cpp \
	customwebpage.cpp

HEADERS += \
	snapshot.h \
	customwebpage.h
