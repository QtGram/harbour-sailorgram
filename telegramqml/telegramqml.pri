QT += qml quick sql xml multimedia
DEFINES += TELEGRAMQML_EMBEDED_MODE

win32 {
    QT += winextras
    LIBS += -LD:/Projects/cutegram-deps/lib -lssleay32 -lcrypto -lz -lqtelegram-ae
    INCLUDEPATH += D:/Projects/cutegram-deps/include D:/Projects/libqtelegram-aseman-edition
} else {
macx {
    QT += macextras
    LIBS += -lssl -lcrypto -lz -L/Users/bardia/Projects/builds/64/lib/ -lqtelegram-ae
    INCLUDEPATH += /Users/bardia/Projects/builds/64/include/libqtelegram-ae
} else {
openbsd {
    LIBS += -lssl -lcrypto -lz -lqtelegram-ae
    INCLUDEPATH += /usr/local/include/libqtelegram-ae $$OUT_PWD/$$DESTDIR/include/libqtelegram-ae
} else {
    LIBS += -lssl -lcrypto -lz -lqtelegram-ae
    INCLUDEPATH += /usr/include/libqtelegram-ae $$OUT_PWD/$$DESTDIR/include/libqtelegram-ae
}
}
}

# Input
SOURCES += \
    $$PWD/telegramqml_plugin.cpp \
    $$PWD/backgroundmanager.cpp \
    $$PWD/chatparticipantlist.cpp \
    $$PWD/database.cpp \
    $$PWD/databasecore.cpp \
    $$PWD/dialogfilesmodel.cpp \
    $$PWD/mp3converterengine.cpp \
    $$PWD/photosizelist.cpp \
    $$PWD/profilesmodel.cpp \
    $$PWD/telegramsearchmodel.cpp \
    $$PWD/telegramuploadsmodel.cpp \
    $$PWD/telegramwallpapersmodel.cpp \
    $$PWD/usernamefiltermodel.cpp \
    $$PWD/telegramqml.cpp \
    $$PWD/tagfiltermodel.cpp \
    $$PWD/telegramchatparticipantsmodel.cpp \
    $$PWD/telegramcontactsmodel.cpp \
    $$PWD/telegramdialogsmodel.cpp \
    $$PWD/telegramfilehandler.cpp \
    $$PWD/telegrammessagesmodel.cpp \
    $$PWD/newsletterdialog.cpp \
    $$PWD/userdata.cpp \
    $$PWD/telegramqmlinitializer.cpp

HEADERS += \
    $$PWD/telegramqml_plugin.h \
    $$PWD/backgroundmanager.h \
    $$PWD/chatparticipantlist.h \
    $$PWD/database.h \
    $$PWD/databasecore.h \
    $$PWD/dialogfilesmodel.h \
    $$PWD/mp3converterengine.h \
    $$PWD/photosizelist.h \
    $$PWD/profilesmodel.h \
    $$PWD/telegramsearchmodel.h \
    $$PWD/telegramuploadsmodel.h \
    $$PWD/telegramwallpapersmodel.h \
    $$PWD/userdata.h \
    $$PWD/usernamefiltermodel.h \
    $$PWD/telegramqml.h \
    $$PWD/tagfiltermodel.h \
    $$PWD/telegramchatparticipantsmodel.h \
    $$PWD/telegramcontactsmodel.h \
    $$PWD/telegramdialogsmodel.h \
    $$PWD/telegramfilehandler.h \
    $$PWD/telegrammessagesmodel.h \
    $$PWD/objects/types.h \
    $$PWD/telegramqml_macros.h \
    $$PWD/telegramqml_global.h \
    $$PWD/newsletterdialog.h \
    $$PWD/telegramqmlinitializer.h

RESOURCES += \
    $$PWD/resource.qrc
