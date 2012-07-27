QT       += core gui

TEMPLATE = lib
TARGET=blockbase

# Xenomai has a variable called signals.
CONFIG += no_keywords

# This is going to statically included by blocks, so must be compiled statically
CONFIG += staticlib

DESTDIR      = build
OBJECTS_DIR = $${DESTDIR}/.obj/
MOC_DIR = $${DESTDIR}/.moc/
RCC_DIR = $${DESTDIR}/.rcc/
UI_DIR = $${DESTDIR}/.ui/

CLEAN_FILES += $${DESTDIR}/${TARGET}

SOURCES += blockbase.cpp\
        settings.c\
        strmap.c\
        rt_block_io.c\
        mtrx.c

HEADERS  += blockbase.h\
            settings.h\
            strmap.h\
            rt_block_io.h\
            mtrx.h

INCLUDEPATH += /usr/include/xenomai
LIBS += -lxenomai -lnative -Xlinker -rpath -Xlinker /usr/lib 

QMAKE_CXXFLAGS += -D_GNU_SOURCE -D_REENTRANT -D__XENO__ -pipe
QMAKE_CXXFLAGS_WARN_ON += -Wextra -Wundef -Wpointer-arith -Wcast-align\
-Wwrite-strings -Wcast-qual -Wswitch-enum -Wformat=2 -Wctor-dtor-privacy\
-Wdisabled-optimization -Wmissing-include-dirs -Woverloaded-virtual\
-Wsign-promo -Wstrict-null-sentinel -Wno-unused
QMAKE_CXXFLAGS_WARN_ON +=-fdiagnostics-show-option
