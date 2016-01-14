TARGET  = qwebp

PLUGIN_TYPE = imageformats
PLUGIN_CLASS_NAME = QWebpPlugin
load(qt_plugin)

CONFIG -= import_plugins
HEADERS += qwebphandler_p.h
SOURCES += main.cpp qwebphandler.cpp
OTHER_FILES += webp.json

config_libwebp {
    unix|win32-g++*: LIBS += -lwebp
    else:win32: LIBS += libwebp.lib
} else {
    include($$PWD/libwebp.pri)
}

INSTALLS = # None
