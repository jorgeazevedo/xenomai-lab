#-------------------------------------------------
#
# Project created by QtCreator 2011-10-01T21:00:51
#
#-------------------------------------------------

QT       += core gui

TARGET = XenomaiLab
TEMPLATE = app

CONFIG += static

#DEFINES += QDEBUG = \"qDebug()\"
#DEFINES = QDEBUG qDebug()

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
