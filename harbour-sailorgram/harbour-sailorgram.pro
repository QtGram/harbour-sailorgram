# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-sailorgram

CONFIG += sailfishapp
QT += sql

# LibQTelegram-AE
libqtelegram_ae.files = $$OUT_PWD/../libqtelegram-ae/*.so*
libqtelegram_ae.path = /usr/share/$$TARGET/lib
INSTALLS += libqtelegram_ae

# TelegramQml
libtelegram_qml.files = $$OUT_PWD/../telegramqml/libtelegramqml.so \
                        $$OUT_PWD/../telegramqml/qmldir

libtelegram_qml.path = /usr/share/$$TARGET/harbour/sailorgram/TelegramQml
INSTALLS += libtelegram_qml

# Telegram Public Key
server_pub.files = $$PWD/server.pub
server_pub.path = /usr/share/$$TARGET
INSTALLS += server_pub

SOURCES += src/harbour-sailorgram.cpp \
    src/localstorage/telegramlocalstorage.cpp \
    src/telegramcalendar.cpp

OTHER_FILES += qml/harbour-sailorgram.qml \
    qml/cover/CoverPage.qml \
    rpm/harbour-sailorgram.changes.in \
    rpm/harbour-sailorgram.spec \
    rpm/harbour-sailorgram.yaml \
    translations/*.ts \
    harbour-sailorgram.desktop \
    qml/pages/ConnectionPage.qml \
    qml/pages/ContactsPage.qml \
    qml/models/Settings.qml \
    server.pub \
    qml/pages/PhoneNumberPage.qml \
    qml/pages/AuthorizationPage.qml \
    qml/js/Settings.js \
    qml/items/ContactItem.qml \
    qml/items/UserAvatar.qml \
    qml/js/CountryList.js \
    qml/pages/DialogsPage.qml \
    qml/components/MessageBar.qml \
    qml/pages/DialogPage.qml \
    qml/items/ChatItem.qml \
    qml/items/messageitem/MessageItem.qml \
    qml/js/TelegramHelper.js \
    qml/items/messageitem/MessageText.qml \
    qml/items/messageitem/MessageMediaItem.qml \
    qml/items/messageitem/media/MessagePhoto.qml \
    qml/items/messageitem/media/MessageDocument.qml

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/harbour-sailorgram-de.ts

HEADERS += \
    src/localstorage/telegramlocalstorage.h \
    src/telegramcalendar.h

RESOURCES += \
    resources.qrc

