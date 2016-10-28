LIBQTELEGRAM_LIB_DIR = $$OUT_PWD/../LibQTelegram
LIBQTELEGRAM_INCLUDE_PATH = $$PWD/LibQTelegram

CONFIG += c++11

!isEqual(TARGET, "harbour-sailorgram") {
    target = $$TARGET
    target.path = /usr/share/harbour-sailorgram/lib
    INSTALLS += target
}
