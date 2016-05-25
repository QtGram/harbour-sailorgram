LIBQTELEGRAM_LIB_DIR = $$OUT_PWD/../libqtelegram-aseman-edition
LIBQTELEGRAM_INCLUDE_PATH = $$PWD/libqtelegram-aseman-edition
TELEGRAMQML_LIB_DIR = $$OUT_PWD/../TelegramQML
TELEGRAMQML_INCLUDE_PATH = $$PWD/TelegramQML
BUILD_MODE += lib

CONFIG += no_install c++11

!isEqual(TARGET, "harbour-sailorgram") {
    target = $$TARGET
    target.path = /usr/share/harbour-sailorgram/lib
    INSTALLS += target
}

isEqual(TARGET, "qtelegram-ae") {
    CONFIG += typeobjects
}
