#-------------------------------------------------
#
# Project created by QtCreator 2013-03-25T03:11:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -lX11

TARGET = Open
TEMPLATE = app


SOURCES += main.cpp\
    OWindow.cpp \
    Cloud.cpp \
    skyItem.cpp \
    Sky.cpp \
    scopeBrowser.cpp \
    eventHandler.cpp

HEADERS  += \
    Common.hpp \
    OWindow.hpp \
    Cloud.hpp \
    skyItem.hpp \
    Sky.hpp \
    scopeBrowser.hpp \
    eventHandler.hpp

FORMS    += Open.ui

OTHER_FILES += \
    README.txt \
    notes.txt
