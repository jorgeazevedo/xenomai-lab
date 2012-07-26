#-------------------------------------------------
#
# Project created by QtCreator 2011-11-13T21:10:00
#
#-------------------------------------------------

QT       += core gui

TARGET = gain_settings
TEMPLATE = app

# Xenomai has a variable called signals.
CONFIG += no_keywords

INCLUDEDIR   = $$(HOME)/.xenomailab/include/
DESTDIR      = ../
OBJECTS_DIR = build/.obj/
MOC_DIR = build/.moc/
RCC_DIR = build/.rcc/
UI_DIR = build/.ui/

CLEAN_FILES += ${TARGET}

SOURCES += main.cpp\
        mainwindow.cpp\
        $${INCLUDEDIR}blockbase.cpp\
        $${INCLUDEDIR}settings.c\
        $${INCLUDEDIR}strmap.c\
        $${INCLUDEDIR}rt_block_io.c\
        $${INCLUDEDIR}mtrx.c\
        $${DESTDIR}gain_settings.c\


HEADERS  += mainwindow.h\
            $${INCLUDEDIR}blockbase.h\
            $${INCLUDEDIR}settings.h\
            $${INCLUDEDIR}strmap.h\
            $${INCLUDEDIR}rt_block_io.h\
            $${INCLUDEDIR}mtrx.h\
            $${DESTDIR}gain_settings.h

INCLUDEPATH += $$INCLUDEDIR $$DESTDIR /usr/include/xenomai
LIBS += -lxenomai -lnative -Xlinker -rpath -Xlinker /usr/lib 

QMAKE_CXXFLAGS += -D_GNU_SOURCE -D_REENTRANT -D__XENO__ -pipe
QMAKE_CXXFLAGS_WARN_ON += -Wextra -Wundef -Wpointer-arith -Wcast-align -Wwrite-strings -Wcast-qual -Wswitch-enum -Wformat=2 -Wctor-dtor-privacy -Wdisabled-optimization -Wmissing-include-dirs -Woverloaded-virtual -Wsign-promo -Wstrict-null-sentinel -Wno-unused
QMAKE_CXXFLAGS_WARN_ON +=-fdiagnostics-show-option
