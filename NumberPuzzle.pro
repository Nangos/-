#-------------------------------------------------
#
# Project created by QtCreator 2016-09-28T15:30:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NumberPuzzle
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    dialogmp.cpp \
    dialogip.cpp \
    dialogap.cpp \
    dialoghp.cpp \
    dialogms.cpp \
    dialogsol.cpp \
    secretary.cpp \
    core.cpp \
    dialogpuz.cpp

HEADERS  += widget.h \
    dialogmp.h \
    dialogip.h \
    dialogap.h \
    dialoghp.h \
    dialogms.h \
    dialogsol.h \
    secretary.h \
    core.h \
    dialogpuz.h

FORMS    += widget.ui \
    dialogmp.ui \
    dialogip.ui \
    dialogap.ui \
    dialoghp.ui \
    dialogms.ui \
    dialogsol.ui \
    dialogpuz.ui
