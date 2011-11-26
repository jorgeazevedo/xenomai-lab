#-------------------------------------------------
#
# Project created by QtCreator 2011-11-13T21:10:00
#
#-------------------------------------------------

QT       += core gui

TARGET = setpoint_settings
TEMPLATE = app

#xenomai has a variable called signals.
CONFIG += no_keywords

INCLUDEDIR   = $$(HOME)/.xenomailab/include/
DESTDIR      = $${PWD}/../../
OBJECTS_DIR = ../$${TARGET}-build-desktop/
MOC_DIR = ../$${TARGET}-build-desktop/
RCC_DIR = ../$${TARGET}-build-desktop/
CLEAN_FILES += ${TARGET}

SOURCES += main.cpp\
        mainwindow.cpp\
        $${INCLUDEDIR}blockbase.cpp\
        $${INCLUDEDIR}settings.c\
        $${INCLUDEDIR}strmap.c\
        $${INCLUDEDIR}rt_block_io.c\
        $${INCLUDEDIR}mtrx.c\
        $${DESTDIR}setpoint_settings.c\


HEADERS  += mainwindow.h\
            $${INCLUDEDIR}blockbase.h\
            $${INCLUDEDIR}settings.h\
            $${INCLUDEDIR}strmap.h\
            $${INCLUDEDIR}rt_block_io.h\
            $${INCLUDEDIR}mtrx.h\
            $${DESTDIR}setpoint_settings.h

INCLUDEPATH +=  $$INCLUDEDIR $$DESTDIR /usr/include/xenomai
LIBS += -L/usr/xenomai/lib -lxenomai -lpthread -lrt -lnative -Xlinker -rpath -Xlinker /usr/lib

QMAKE_CXXFLAGS += -D_GNU_SOURCE -D_REENTRANT -Wall -pipe -D__XENO__
