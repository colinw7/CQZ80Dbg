TEMPLATE = app

TARGET = CQZ80Dbg

QT += widgets

DEPENDPATH += .

INCLUDEPATH += . ../include

QMAKE_CXXFLAGS += -std=c++11

# Input
SOURCES += \
main.cpp \
CQZ80Dbg.cpp \

HEADERS += \
CQZ80Dbg.h

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
../include \
../../CStrUtil/include \
../../CZ80/include \
../../CUtil/include \

unix:LIBS += \
-L$$LIB_DIR \
-L../../CZ80/lib \
-L../../CFile/lib \
-L../../COS/lib \
-L../../CStrUtil/lib \
-lCZ80 \
-lCFile \
-lCOS \
-lCStrUtil
