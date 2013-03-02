# qmake CONFIG+=<debug|release>

QT += core webkit network

PROJECT  = webtaku
LANGUAGE = C++
TEMPLATE = app
LOCATION = /usr/local/
#TARGET   = <target_file>


SOURCES += \
	main.cpp \
	gyotaku.cpp \
	customwebpage.cpp \
	customwebview.cpp

HEADERS += \
	gyotaku.h \
	customwebpage.h \
	customwebview.h


# configuration
CONFIG += console warn_on
CONFIG -= app_bundle

QMAKE_CFLAGS   += -ansi
QMAKE_CXXFLAGS += -ansi
#QMAKE_CXXFLAGS += -std=c++0x

CONFIG(debug, debug|release) {
	QMAKE_CFLAGS   += -O0 -pg
	QMAKE_CXXFLAGS += -O0 -pg
	QMAKE_LFLAGS   += -O0 -pg
}
CONFIG(release, debug|release) {
	QMAKE_CFLAGS   += -Ofast
	QMAKE_CXXFLAGS += -Ofast
	QMAKE_LFLAGS   += -Ofast

# installation
	target.path  = $${LOCATION}bin
	target.files = $${TARGET}
#	target.extra = <commands>

	source.path  = $${LOCATION}src/$${PROJECT}
	source.files = $${SOURCES} $${HEADERS} $${PROJECT}.pro LICENSE README.md man
#	source.extra = <commands>

	man.path  = $${LOCATION}share/man
	man.files = man/*
#	man.extra = <commands>

	INSTALLS += target source man
}