#-------------------------------------------------
#
# Project created by QtCreator 2011-11-13T21:10:00
#
#-------------------------------------------------

QT       += core gui

TARGET = DA_servo_settings
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
        $${DESTDIR}DA_servo_settings.c\


HEADERS  += mainwindow.h\
            $${INCLUDEDIR}blockbase.h\
            $${INCLUDEDIR}settings.h\
            $${INCLUDEDIR}strmap.h\
            $${INCLUDEDIR}rt_block_io.h\
            $${INCLUDEDIR}mtrx.h\
            $${DESTDIR}DA_servo_settings.h

INCLUDEPATH +=  $$INCLUDEDIR $$DESTDIR /usr/include/xenomai
LIBS += -L/usr/lib -lxenomai -lpthread -lnative -Xlinker -rpath -Xlinker /usr/lib

QMAKE_CXXFLAGS += -D_GNU_SOURCE -D_REENTRANT -Wall -pipe -D__XENO__
