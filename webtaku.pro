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
		isEmpty(PREFIX) {
			PREFIX = /usr/local
		}
		isEmpty(BASH_COMPLETION_DIR) {
			BASH_COMPLETION_DIR = /etc/bash_completion.d
		}

		target.path  = $${PREFIX}/bin
		target.files = $${TARGET}
#		target.extra = <commands>

		source.path  = $${PREFIX}/src/$${PROJECT}
		source.files = $${SOURCES} $${HEADERS} $${PROJECT}.pro LICENSE README.md files
#		source.extra = <commands>

		man.path  = $${PREFIX}/share/man
		man.files = files/prefix/share/man/*
#		man.extra = <commands>

		completion.path  = $${BASH_COMPLETION_DIR}/
		completion.files = files/completion/bash/*
#		completion.extra = <commands>

		INSTALLS += target source man completion
	}
}
