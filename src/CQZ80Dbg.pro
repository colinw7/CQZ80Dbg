TEMPLATE = lib

TARGET = CQZ80Dbg

DEPENDPATH += .

QT += widgets

CONFIG += staticlib
CONFIG += debug

QMAKE_CXXFLAGS += -std=c++14

MOC_DIR = .moc

# Input
SOURCES += \
CQZ80Dbg.cpp \
CQZ80Instructions.cpp \
CQZ80Memory.cpp \
CQZ80RegEdit.cpp \
CQZ80Stack.cpp \
CQZ80TraceBack.cpp \

HEADERS += \
../include/CQZ80Dbg.h \
CQZ80Instructions.h \
CQZ80Memory.h \
CQZ80RegEdit.h \
CQZ80Stack.h \
CQZ80TraceBack.h \

OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

DESTDIR = ../lib

INCLUDEPATH += \
../include \
../../CStrUtil/include \
../../CZ80/include \
../../CUtil/include \
