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

DESTDIR = build/
OBJECTS_DIR = build/.obj
MOC_DIR = build/.moc
RCC_DIR = build/.rcc
UI_DIR = build/.ui

SOURCES += src/main.cpp\
           src/mainwindow.cpp \
           src/diagramitem.cpp \
           src/diagramscene.cpp \
           src/diagramtextitem.cpp \
           src/arrow.cpp \
           src/blockdiagram.cpp \
           src/block.cpp \
           src/processresult.cpp \
           src/settings.cpp \
           src/strmap.cpp \
           src/mtrx.c \
           src/line.cpp \
           src/filesettings.cpp \
           src/projectfile.cpp \
           src/blockregistry.cpp \
           src/superblock.cpp \
           src/workspace.cpp \
           src/superline.cpp \
           src/linepath.cpp

HEADERS  += src/mainwindow.h \
            src/diagramitem.h \
            src/diagramscene.h \
            src/diagramtextitem.h \
            src/arrow.h \
            src/blockdiagram.h \
            src/block.h \
            src/processresult.h \
            src/settings.h \
            src/strmap.h \
            src/mtrx.h \
            src/line.h \
            src/filesettings.h \
            src/projectfile.h \
            src/blockregistry.h \
            src/superblock.h \
            src/workspace.h \
            src/superline.h \
            src/linepath.h \
            src/macros.h

FORMS    += src/mainwindow.ui

RESOURCES     = src/xenomailab.qrc

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
