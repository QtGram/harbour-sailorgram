QT += gui
QT += network
QT += multimedia

win32 {
    isEmpty(OPENSSL_LIB_DIR): OPENSSL_LIB_DIR = $${DESTDIR}
    isEmpty(OPENSSL_INCLUDE_PATH): OPENSSL_INCLUDE_PATH = $${DESTDIR}/include

    LIBS += -L$${OPENSSL_LIB_DIR} -lssleay32
    INCLUDEPATH += $${OPENSSL_INCLUDE_PATH}
    
    win32-msvc* {
        LIBS += -llibeay32 -lzlibstat -lUser32 -lAdvapi32 -lGdi32
    } else {
        LIBS += -lcrypto -lz
    }
} else {
    isEmpty(OPENSSL_INCLUDE_PATH): OPENSSL_INCLUDE_PATH = /usr/include/ /usr/local/include/
    isEmpty(OPENSSL_LIB_DIR) {
        LIBS += -lssl -lcrypto -lz
    } else {
        LIBS += -L$${OPENSSL_LIB_DIR} -lssl -lcrypto -lz
    }

    INCLUDEPATH += $${OPENSSL_INCLUDE_PATH}
}

include(telegram/telegram.pri)

SOURCES += \
    $$PWD/util/utils.cpp \
    $$PWD/util/cryptoutils.cpp \
    $$PWD/util/asserter.cpp \
    $$PWD/file/downloadfile.cpp \
    $$PWD/file/filehandler.cpp \
    $$PWD/secret/decrypter.cpp \
    $$PWD/secret/secretstate.cpp \
    $$PWD/secret/secretchat.cpp \
    $$PWD/secret/encrypter.cpp \
    $$PWD/secret/decryptedmessagebuilder.cpp \
    $$PWD/secret/decryptedmessage.cpp \
    $$PWD/secret/decryptedmessageaction.cpp \
    $$PWD/secret/decryptedmessagemedia.cpp \
    $$PWD/core/outboundpkt.cpp \
    $$PWD/core/eventtimer.cpp \
    $$PWD/core/settings.cpp \
    $$PWD/core/dcprovider.cpp \
    $$PWD/core/sessionmanager.cpp \
    $$PWD/core/inboundpkt.cpp \
    $$PWD/core/abstractapi.cpp \
    $$PWD/core/connection.cpp \
    $$PWD/core/query.cpp \
    $$PWD/core/session.cpp \
    $$PWD/core/dcauth.cpp \
    $$PWD/telegram.cpp \
    $$PWD/file/uploadfileengine.cpp

HEADERS += \
    $$PWD/util/cryptoutils.h \
    $$PWD/util/utils.h \
    $$PWD/util/tlvalues.h \
    $$PWD/util/constants.h \
    $$PWD/util/asserter.h \
    $$PWD/file/filehandler.h \
    $$PWD/file/file.h \
    $$PWD/file/downloadfile.h \
    $$PWD/file/fileoperation.h \
    $$PWD/secret/secretchatmessage.h \
    $$PWD/secret/decryptedmessagebuilder.h \
    $$PWD/secret/decrypter.h \
    $$PWD/secret/secretchat.h \
    $$PWD/secret/encrypter.h \
    $$PWD/secret/secretstate.h \
    $$PWD/secret/decryptedmessage.h \
    $$PWD/secret/decryptedmessageaction.h \
    $$PWD/secret/decryptedmessagemedia.h \
    $$PWD/core/query.h \
    $$PWD/core/session.h \
    $$PWD/core/connection.h \
    $$PWD/core/settings.h \
    $$PWD/core/outboundpkt.h \
    $$PWD/core/querymethods.h \
    $$PWD/core/sessionmanager.h \
    $$PWD/core/dcprovider.h \
    $$PWD/core/abstractapi.h \
    $$PWD/core/inboundpkt.h \
    $$PWD/core/dcauth.h \
    $$PWD/core/endpoint.h \
    $$PWD/core/eventtimer.h \
    $$PWD/core/dc.h \
    $$PWD/telegram.h \
    $$PWD/libqtelegram_global.h \
    $$PWD/file/uploadfileengine.h
