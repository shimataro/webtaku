# qmake CONFIG+=<debug|release>

QT += core webkit network

LANGUAGE = C++
TEMPLATE = app
#TARGET   = <target_file>


SOURCES += \
	main.cpp \
	snapshot.cpp \
	customwebpage.cpp

HEADERS += \
	snapshot.h \
	customwebpage.h


#######################################
# configuration
CONFIG += console warn_on
CONFIG -= app_bundle

QMAKE_CFLAGS   += -ansi
QMAKE_CXXFLAGS += -ansi
#QMAKE_CXXFLAGS += -std=c++0x

CONFIG(debug, debug|release) {
	QMAKE_CFLAGS   += -O0
	QMAKE_CXXFLAGS += -O0
}
CONFIG(release, debug|release) {
	QMAKE_CFLAGS   += -Ofast
	QMAKE_CXXFLAGS += -Ofast
}


#######################################
# installation
target.path  = /usr/local/bin
target.files = $${TARGET}
#target.extra = <commands>

source.path  = /usr/local/src/$${TARGET}
source.files = *.cpp *.h *.pro LICENSE README.md
#source.extra = <commands>

INSTALLS += target source
