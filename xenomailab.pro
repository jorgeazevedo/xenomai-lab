#-------------------------------------------------
#
# Project created by QtCreator 2011-10-01T21:00:51
#
#-------------------------------------------------

QT       += core gui

TEMPLATE = app
TARGET = xenomailab

VERSION = 1.0.0

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



# Installation

#added the -a. this retains the owner of the files no copy!
QMAKE_INSTALL_DIR = cp -a -f -r

#This is the propper way to install the final executable
target.path = /usr/bin
INSTALLS += target

doc.files += data/doc/*
doc.path = /usr/share/doc/xenomailab/
INSTALLS += doc

man.files += data/man/xenomailab.1.gz
man.path = /usr/share/man/man1/
INSTALLS += man

shortcut.files += data/xenomailab.desktop
shortcut.path = /usr/share/applications/
INSTALLS += shortcut

icons.files += data/icons/48x48/apps/xenomailab.png
icons.path = /usr/share/icons/hicolor/48x48/apps
#INSTALLS += icons

pixmap.files += data/pixmap/*
pixmap.path = /usr/share/pixmaps/
INSTALLS += pixmap

homefolder.files += data/.xenomailab
homefolder.path = $$(HOME)
INSTALLS += homefolder

#unix:configfiles.extra = chmod +x data/config/WallpaperChanger/Scripts/*; make clean; mv -f wallch-1 wallch
#configfiles.files += data/config/*
#configfiles.path = /usr/share/
#INSTALLS += configfiles
