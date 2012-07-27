QT       += core gui
TEMPLATE = app

CLEAN_FILES += ${TARGET}

# Xenomai has a variable called signals.
CONFIG += no_keywords


INCLUDEDIR   = $$(HOME)/.xenomailab/include
DESTDIR      = ..
BUILDDIR      = build
OBJECTS_DIR = $${BUILDDIR}/.obj/
MOC_DIR = $${BUILDDIR}/.moc/
RCC_DIR = $${BUILDDIR}/.rcc/
UI_DIR = $${BUILDDIR}/.ui/

# We want the blockbase lib to the staticaly compiled
CONFIG += static
LIBS += -L$${INCLUDEDIR}/build/ -lblockbase
# Rebuild target if blockbase.h/.cpp is changed
PRE_TARGETDEPS += $${INCLUDEDIR}/build/libblockbase.a

SOURCES += main.cpp\
        mainwindow.cpp\
        $${DESTDIR}/$${TARGET}.c


HEADERS  += mainwindow.h\
            $${DESTDIR}/$${TARGET}.h
            

INCLUDEPATH += $$INCLUDEDIR $$DESTDIR /usr/include/xenomai
LIBS += -lxenomai -lnative -Xlinker -rpath -Xlinker /usr/lib 

QMAKE_CXXFLAGS += -D_GNU_SOURCE -D_REENTRANT -D__XENO__ -pipe
QMAKE_CXXFLAGS_WARN_ON += -Wextra -Wundef -Wpointer-arith -Wcast-align\
-Wwrite-strings -Wcast-qual -Wswitch-enum -Wformat=2 -Wctor-dtor-privacy\
-Wdisabled-optimization -Wmissing-include-dirs -Woverloaded-virtual\
-Wsign-promo -Wstrict-null-sentinel -Wno-unused
QMAKE_CXXFLAGS_WARN_ON +=-fdiagnostics-show-option

##
#WARNINGS += -Wall

