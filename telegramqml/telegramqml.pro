TEMPLATE = lib
TARGET = telegramqml
CONFIG += qt
QT += qml quick sql xml multimedia
DEFINES += TELEGRAMQML_LIBRARY

uri = TelegramQml

LIBS += -L$$OUT_PWD/../libqtelegram-ae -lssl -lcrypto -lz -lqtelegram-ae
INCLUDEPATH += $$PWD/../libqtelegram-ae

# Input
SOURCES += \
    telegramqml_plugin.cpp \
    backgroundmanager.cpp \
    chatparticipantlist.cpp \
    database.cpp \
    databasecore.cpp \
    dialogfilesmodel.cpp \
    mp3converterengine.cpp \
    photosizelist.cpp \
    profilesmodel.cpp \
    telegramsearchmodel.cpp \
    telegramuploadsmodel.cpp \
    telegramwallpapersmodel.cpp \
    usernamefiltermodel.cpp \
    telegramqml.cpp \
    tagfiltermodel.cpp \
    telegramchatparticipantsmodel.cpp \
    telegramcontactsmodel.cpp \
    telegramdialogsmodel.cpp \
    telegramfilehandler.cpp \
    telegrammessagesmodel.cpp \
    newsletterdialog.cpp \
    userdata.cpp \
    telegramqmlinitializer.cpp

HEADERS += \
    telegramqml_plugin.h \
    backgroundmanager.h \
    chatparticipantlist.h \
    database.h \
    databasecore.h \
    dialogfilesmodel.h \
    mp3converterengine.h \
    photosizelist.h \
    profilesmodel.h \
    telegramsearchmodel.h \
    telegramuploadsmodel.h \
    telegramwallpapersmodel.h \
    userdata.h \
    usernamefiltermodel.h \
    telegramqml.h \
    tagfiltermodel.h \
    telegramchatparticipantsmodel.h \
    telegramcontactsmodel.h \
    telegramdialogsmodel.h \
    telegramfilehandler.h \
    telegrammessagesmodel.h \
    objects/types.h \
    telegramqml_macros.h \
    telegramqml_global.h \
    newsletterdialog.h \
    telegramqmlinitializer.h

RESOURCES += \
    resource.qrc

linux {
    contains(QMAKE_HOST.arch, x86_64) {
        LIB_PATH = x86_64-linux-gnu
    } else {
        LIB_PATH = i386-linux-gnu
    }
}

#contains(BUILD_MODE,lib) {
#    DEFINES += BUILD_MODE_LIB
#    isEmpty(PREFIX) {
#        PREFIX = /usr
#    }

#    INSTALL_PREFIX = $$PREFIX/include/telegramqml
#    INSTALL_HEADERS = $$HEADERS
#    include(qmake/headerinstall.pri)

#    target = $$TARGET
#    target.path = $$PREFIX/lib/$$LIB_PATH

#    INSTALLS += target

#} else {
    CONFIG += plugin
    DEFINES += BUILD_MODE_PLUGIN

#    TARGET = $$qtLibraryTarget($$TARGET)
    DISTFILES = qmldir
    !equals(_PRO_FILE_PWD_, $$OUT_PWD) {
        copy_qmldir.target = $$OUT_PWD/qmldir
        copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
        copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
        QMAKE_EXTRA_TARGETS += copy_qmldir
        PRE_TARGETDEPS += $$copy_qmldir.target
    }

#    qmldir.files = qmldir
#    unix {
#        installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
#        qmldir.path = $$installPath
#        target.path = $$installPath
#        INSTALLS += target qmldir
#    }
#}

