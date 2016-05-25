#-------------------------------------------------
#
# Project created by QtCreator 2015-07-17T13:14:02
#
#-------------------------------------------------

QT       += core
QT       -= gui
VERSION = 1.0.0
CONFIG += c++11

TARGET = libqtelegram-generator

TEMPLATE = app


SOURCES += main.cpp \
    typegenerator.cpp \
    functiongenerator.cpp \
    abstractgenerator.cpp \
    coretypesgenerator.cpp \
    typeobjectgenerator.cpp \
    apigenerator.cpp \
    telegramcoregenerator.cpp \
    lqtgtypegenerator.cpp \
    documentsgenerator.cpp \
    databasegenerator.cpp

HEADERS += \
    typegenerator.h \
    functiongenerator.h \
    abstractgenerator.h \
    coretypesgenerator.h \
    typeobjectgenerator.h \
    apigenerator.h \
    telegramcoregenerator.h \
    lqtgtypegenerator.h \
    documentsgenerator.h \
    databasegenerator.h

RESOURCES += \
    resource.qrc
