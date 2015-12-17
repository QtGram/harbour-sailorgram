QT += qml quick sql xml multimedia

contains(DEFINES, UBUNTU_PHONE): CONFIG += c++11

win32 {
    isEmpty(OPENSSL_LIB_DIR): OPENSSL_LIB_DIR = $${DESTDIR}
    isEmpty(LIBQTELEGRAM_LIB_DIR): LIBQTELEGRAM_LIB_DIR = $$[QT_INSTALL_LIBS] $${DESTDIR}
    isEmpty(OPENSSL_INCLUDE_PATH): OPENSSL_INCLUDE_PATH = $${DESTDIR}/include
    isEmpty(LIBQTELEGRAM_INCLUDE_PATH): LIBQTELEGRAM_INCLUDE_PATH = $$[QT_INSTALL_HEADERS]/libqtelegram-ae $${DESTDIR}/include/libqtelegram-ae

    LIBS += -L$${OPENSSL_LIB_DIR} -lssleay32 -lcrypto -lz -L$${LIBQTELEGRAM_LIB_DIR} -lqtelegram-ae
    INCLUDEPATH += $${OPENSSL_INCLUDE_PATH} $${LIBQTELEGRAM_INCLUDE_PATH}
} else {
    isEmpty(OPENSSL_INCLUDE_PATH): OPENSSL_INCLUDE_PATH = /usr/include /usr/local/include
    isEmpty(LIBQTELEGRAM_INCLUDE_PATH): LIBQTELEGRAM_INCLUDE_PATH = $$[QT_INSTALL_HEADERS]/libqtelegram-ae /usr/include/libqtelegram-ae /usr/local/include/libqtelegram-ae
    isEmpty(OPENSSL_LIB_DIR) {
        LIBS += -lssl -lcrypto -lz
    } else {
        LIBS += -L$${OPENSSL_LIB_DIR} -lssl -lcrypto -lz
    }
    isEmpty(LIBQTELEGRAM_LIB_DIR) {
        LIBS += -lqtelegram-ae
    } else {
        LIBS += -L$${LIBQTELEGRAM_LIB_DIR} -lqtelegram-ae
    }

    INCLUDEPATH += $${LIBQTELEGRAM_INCLUDE_PATH} $${OPENSSL_INCLUDE_PATH}
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
    $$PWD/telegramdetailedcontactsmodel.cpp \
    $$PWD/telegramcontactsfiltermodel.cpp \
    $$PWD/telegramdialogsmodel.cpp \
    $$PWD/telegramfilehandler.cpp \
    $$PWD/telegrammessagesmodel.cpp \
    $$PWD/telegramthumbnailer.cpp \
    $$PWD/telegramthumbnailercore.cpp \
    $$PWD/newsletterdialog.cpp \
    $$PWD/userdata.cpp \
    $$PWD/telegramqmlinitializer.cpp \
    $$PWD/tqobject.cpp \
    $$PWD/stickersmodel.cpp \
    $$PWD/documentattributelist.cpp \
    $$PWD/tgabstractlistmodel.cpp \
    $$PWD/databaseabstractencryptor.cpp

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
    $$PWD/telegramdetailedcontactsmodel.h \
    $$PWD/telegramcontactsfiltermodel.h \
    $$PWD/telegramdialogsmodel.h \
    $$PWD/telegramfilehandler.h \
    $$PWD/telegrammessagesmodel.h \
    $$PWD/telegramthumbnailer.h \
    $$PWD/telegramthumbnailercore.h \
    $$PWD/objects/types.h \
    $$PWD/telegramqml_macros.h \
    $$PWD/telegramqml_global.h \
    $$PWD/newsletterdialog.h \
    $$PWD/telegramqmlinitializer.h \
    $$PWD/tqobject.h \
    $$PWD/stickersmodel.h \
    $$PWD/documentattributelist.h \
    $$PWD/tgabstractlistmodel.h \
    $$PWD/databaseabstractencryptor.h

RESOURCES += \
    $$PWD/tqmlresource.qrc
