#-------------------------------------------------
#
# Project created by QtCreator 2011-11-13T21:10:00
#
#-------------------------------------------------

QT       += core gui

TARGET = pid_settings
TEMPLATE = app

# Xenomai has a variable called signals.
CONFIG += no_keywords

INCLUDEDIR   = $$(HOME)/.xenomailab/include
DESTDIR      = ..
OBJECTS_DIR =$${INCLUDEDIR}/build/.obj/
MOC_DIR = $${INCLUDEDIR}/build/.moc/
RCC_DIR = $${INCLUDEDIR}/build/.rcc/
UI_DIR = $${INCLUDEDIR}/build/.ui/

CLEAN_FILES += ${TARGET}

SOURCES += main.cpp\
        mainwindow.cpp\
        $${INCLUDEDIR}/blockbase.cpp\
        $${INCLUDEDIR}/settings.c\
        $${INCLUDEDIR}/strmap.c\
        $${INCLUDEDIR}/rt_block_io.c\
        $${INCLUDEDIR}/mtrx.c\
        $${DESTDIR}/pid_settings.c\


HEADERS  += mainwindow.h\
            $${INCLUDEDIR}/blockbase.h\
            $${INCLUDEDIR}/settings.h\
            $${INCLUDEDIR}/strmap.h\
            $${INCLUDEDIR}/rt_block_io.h\
            $${INCLUDEDIR}/mtrx.h\
            $${DESTDIR}/pid_settings.h

INCLUDEPATH += $$INCLUDEDIR $$DESTDIR /usr/include/xenomai
LIBS += -lxenomai -lnative -Xlinker -rpath -Xlinker /usr/lib 

QMAKE_CXXFLAGS += -D_GNU_SOURCE -D_REENTRANT -D__XENO__ -pipe
QMAKE_CXXFLAGS_WARN_ON += -W -Wextra -Wundef -Wpointer-arith -Wcast-align\
-Wwrite-strings -Wcast-qual -Wswitch-enum -Wformat=2 -Wctor-dtor-privacy\
-Wdisabled-optimization -Wmissing-include-dirs -Woverloaded-virtual\
-Wsign-promo -Wstrict-null-sentinel -Wno-unused
QMAKE_CXXFLAGS_WARN_ON +=-fdiagnostics-show-option
