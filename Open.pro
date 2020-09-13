#-------------------------------------------------
#
# Project created by QtCreator 2013-03-25T03:11:17
#
#-------------------------------------------------

QT       += core

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -lX11 -lXtst -lImlib2  -lXcomposite -lXrender -lXfixes  -lXdamage -lXi -lcairo

TARGET = Open
TEMPLATE = app


SOURCES += main.cpp\
    Cloud.cpp \
    skyItem.cpp \
    Sky.cpp \
    dataStructuers.cpp \
    skyScope.cpp \
    Sky-events.cpp \
    Sky-clouds.cpp \
    Sky-ease.cpp \
    skyNotifier.cpp

HEADERS  += \
    Cloud.hpp \
    skyItem.hpp \
    Sky.hpp \
    dataStructuers.hpp \
    Common.hpp \
    skyScope.hpp \
    skyNotifier.hpp

#FORMS    += Open.ui

OTHER_FILES += \
    README.txt \
    notes.txt
    
