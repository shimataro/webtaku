# qmake CONFIG+=<debug|release>

QT += core webkit network

PROJECT  = webtaku
LANGUAGE = C++


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

# installation settings
	unix {
		LOCATION_INSTALLATION = /usr/local/
		LOCATION_COMPLETION = /etc/bash_completion.d/

		target.path  = $${LOCATION_INSTALLATION}bin
		target.files = $${TARGET}
#		target.extra = <commands>

		source.path  = $${LOCATION_INSTALLATION}src/$${PROJECT}
		source.files = $${SOURCES} $${HEADERS} $${PROJECT}.pro LICENSE README.md root
#		source.extra = <commands>

		man.path  = $${LOCATION_INSTALLATION}share/man
		man.files = root$${LOCATION_INSTALLATION}man/*
#		man.extra = <commands>

		completion.path  = $${LOCATION_COMPLETION}
		completion.files = root$${LOCATION_COMPLETION}webtaku
#		completion.extra = <commands>

		INSTALLS += target source man completion
	}
}
