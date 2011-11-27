#-------------------------------------------------
#
# Project created by QtCreator 2011-10-01T21:00:51
#
#-------------------------------------------------

QT       += core gui

TARGET = xenomailab
TEMPLATE = app

CONFIG += static

SOURCES += main.cpp\
        mainwindow.cpp \
    diagramitem.cpp \
    diagramscene.cpp \
    diagramtextitem.cpp \
    arrow.cpp \
    blockdiagram.cpp \
    block.cpp \
    processresult.cpp \
    settings.cpp \
    strmap.cpp \
    mtrx.c \
    line.cpp \
    filesettings.cpp \
    projectfile.cpp \
    blockregistry.cpp \
    superblock.cpp \
    workspace.cpp \
    superline.cpp \
    linepath.cpp

HEADERS  += mainwindow.h \
    diagramitem.h \
    diagramscene.h \
    diagramtextitem.h \
    arrow.h \
    blockdiagram.h \
    block.h \
    processresult.h \
    settings.h \
    strmap.h \
    mtrx.h \
    line.h \
    filesettings.h \
    projectfile.h \
    blockregistry.h \
    superblock.h \
    workspace.h \
    superline.h \
    linepath.h \
    macros.h

FORMS    += mainwindow.ui

RESOURCES     = xenomailab.qrc

QMAKE_CXXFLAGS += -Wno-deprecated

#added the -a. this retains the owner of the files no copy!
QMAKE_INSTALL_DIR = cp -a -f -r

#unix:configfiles.extra = chmod +x data/config/WallpaperChanger/Scripts/*; make clean; mv -f wallch-1 wallch
#configfiles.files += data/config/*
#configfiles.path = /usr/share/

binfile.files += xenomailab
binfile.path = /usr/bin/

docfiles.files += data/doc/*
docfiles.path = /usr/share/doc/xenomailab/

manfiles.files += data/man/xenomailab.1.gz
manfiles.path = /usr/share/man/man1/

shortcutfiles.files += data/xenomailab.desktop
shortcutfiles.path = /usr/share/applications/

iconfiles.files += data/icons/48x48/apps/xenomailab.png
iconfiles.path = /usr/share/icons/hicolor/48x48/apps

pixmapfiles.files += data/pixmap/*
pixmapfiles.path = /usr/share/pixmaps/

homefolder.files += data/xenomailab/*
homefolder.path = $$(HOME)/.xenomailab/

#INSTALLS += configfiles
INSTALLS += binfile
INSTALLS += docfiles
INSTALLS += manfiles
INSTALLS += shortcutfiles
INSTALLS += iconfiles
INSTALLS += pixmapfiles
INSTALLS += homefolder
