# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./src
# Target is an application:  ../bin/akonadi
INCLUDEPATH += ../include ../test searchproviders/ handler/

SOURCES += main.cpp
TEMPLATE = app
CONFIG += debug \
warn_on \
thread \
qt
QT = core network sql
TARGET = akonadi
DESTDIR = ../bin
DEFINES += QT_NO_CAST_TO_ASCII
LIBS = -L../lib -lakonadiprivate
