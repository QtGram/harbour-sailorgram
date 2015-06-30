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
QT += sql dbus

LIBS += -L$$OUT_PWD/../libqtelegram-ae -L$$OUT_PWD/../telegramqml -lssl -lcrypto -lz -lqtelegram-ae -ltelegramqml
INCLUDEPATH += $$PWD/../telegramqml

# TelegramQml
libtelegram_qml.files = $$OUT_PWD/../telegramqml/*.so*
libtelegram_qml.path = /usr/share/$$TARGET/lib
INSTALLS += libtelegram_qml

# LibQTelegram-AE
libqtelegram_ae.files = $$OUT_PWD/../libqtelegram-ae/*.so*
libqtelegram_ae.path = /usr/share/$$TARGET/lib
INSTALLS += libqtelegram_ae

# Telegram Public Key
server_pub.files = $$PWD/server.pub
server_pub.path = /usr/share/$$TARGET
INSTALLS += server_pub

SOURCES += src/harbour-sailorgram.cpp \
    src/localstorage/telegramlocalstorage.cpp \
    src/telegramcalendar.cpp \
    src/dbus/notifications/notifications.cpp

OTHER_FILES += qml/harbour-sailorgram.qml \
    rpm/harbour-sailorgram.changes.in \
    rpm/harbour-sailorgram.spec \
    rpm/harbour-sailorgram.yaml \
    translations/*.ts \
    harbour-sailorgram.desktop \
    qml/pages/ConnectionPage.qml \
    qml/models/Settings.qml \
    server.pub \
    qml/pages/PhoneNumberPage.qml \
    qml/pages/AuthorizationPage.qml \
    qml/js/Settings.js \
    qml/items/user/UserAvatar.qml \
    qml/items/user/UserItem.qml \
    qml/js/CountryList.js \
    qml/components/MessageBar.qml \
    qml/items/ConversationItem.qml \
    qml/items/messageitem/MessageItem.qml \
    qml/js/TelegramHelper.js \
    qml/items/messageitem/MessageText.qml \
    qml/items/messageitem/media/MessageMediaItem.qml \
    qml/items/messageitem/media/MessagePhoto.qml \
    qml/items/messageitem/media/MessageDocument.qml \
    qml/components/PopupMessage.qml \
    qml/menus/ConversationMenu.qml \
    qml/pages/conversations/ConversationsPage.qml \
    qml/pages/conversations/ConversationPage.qml \
    qml/pages/users/UserPage.qml \
    qml/pages/users/UsersPage.qml \
    qml/menus/UserMenu.qml \
    qml/components/ClickableLabel.qml \
    qml/items/messageitem/media/MessageThumbnail.qml \
    qml/js/TelegramConstants.js

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
    src/telegramcalendar.h \
    src/dbus/notifications/notifications.h

RESOURCES += \
    resources.qrc

