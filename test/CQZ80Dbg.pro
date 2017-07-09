TEMPLATE = app

TARGET = CQZ80Dbg

QT += widgets

DEPENDPATH += .

INCLUDEPATH += . ../include

QMAKE_CXXFLAGS += -std=c++11

# Input
SOURCES += \
CQZ80Dbg.cpp \
CQZ80Instructions.cpp \
CQZ80Memory.cpp \
CQZ80RegEdit.cpp \
CQZ80Stack.cpp \
CQZ80TraceBack.cpp \

HEADERS += \
../include/CQZ80Dbg.h

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
