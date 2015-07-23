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
INCLUDEPATH +=  /usr/include/openssl $$PWD/../telegramqml $$PWD/../libqtelegram-ae

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
    src/dbus/notifications/notifications.cpp \
    src/sailorgram.cpp \
    src/filepicker/folderlistmodel.cpp \
    src/heartbeat.cpp \
    src/dbus/screenblank.cpp

OTHER_FILES += qml/harbour-sailorgram.qml \
    rpm/harbour-sailorgram.changes.in \
    rpm/harbour-sailorgram.spec \
    rpm/harbour-sailorgram.yaml \
    translations/*.ts \
    harbour-sailorgram.desktop \
    qml/pages/ConnectionPage.qml \
    server.pub \
    qml/pages/PhoneNumberPage.qml \
    qml/pages/AuthorizationPage.qml \
    qml/js/Settings.js \
    qml/items/user/UserItem.qml \
    qml/js/CountryList.js \
    qml/items/messageitem/MessageBar.qml \
    qml/items/conversation/ConversationItem.qml \
    qml/items/messageitem/MessageItem.qml \
    qml/js/TelegramHelper.js \
    qml/items/messageitem/MessageText.qml \
    qml/items/messageitem/media/MessageMediaItem.qml \
    qml/items/messageitem/media/MessagePhoto.qml \
    qml/items/messageitem/media/MessageDocument.qml \
    qml/components/PopupMessage.qml \
    qml/pages/conversations/ConversationsPage.qml \
    qml/pages/conversations/ConversationPage.qml \
    qml/pages/users/UserPage.qml \
    qml/pages/users/UsersPage.qml \
    qml/menus/UserMenu.qml \
    qml/components/ClickableLabel.qml \
    qml/items/messageitem/media/MessageThumbnail.qml \
    qml/js/TelegramConstants.js \
    qml/pages/picker/FilePickerPage.qml \
    qml/components/pickers/FilePicker.qml \
    qml/pages/SignUpPage.qml \
    qml/items/conversation/ConversationImage.qml \
    qml/items/peer/PeerItem.qml \
    qml/items/peer/PeerImage.qml \
    qml/pages/conversations/ConversationInfoPage.qml \
    qml/items/user/UserInfo.qml \
    qml/items/chat/ChatInfo.qml \
    qml/js/TelegramAction.js \
    qml/pages/chat/AddContactsPage.qml \
    qml/menus/MessageMenu.qml \
    qml/menus/conversation/ConversationMenu.qml \
    qml/pages/chat/CreateChatPage.qml \
    qml/menus/conversation/ConversationsPullDownMenu.qml \
    qml/menus/chat/ChatInfoMenu.qml \
    qml/cover/CoverPage.qml \
    qml/components/cover/CoverLabel.qml \
    qml/components/ConnectingLabel.qml \
    qml/components/telegram/TelegramMenuItem.qml \
    qml/models/Context.qml \
    qml/components/telegram/TelegramPullDownMenu.qml \
    qml/pages/profile/ProfilePage.qml \
    qml/pages/profile/ChangeUsernamePage.qml \
    qml/pages/media/MediaPhotoPage.qml \
    qml/components/mediaplayer/mediacomponents/MediaPlayerCursor.qml \
    qml/components/mediaplayer/mediacomponents/MediaPlayerProgressBar.qml \
    qml/components/mediaplayer/mediacomponents/MediaPlayerTimings.qml \
    qml/components/mediaplayer/mediacomponents/MediaPlayerTitle.qml \
    qml/components/mediaplayer/mediacomponents/MediaPlayerToolBar.qml \
    qml/components/mediaplayer/MediaPlayer.qml \
    qml/pages/media/MediaPlayerPage.qml \
    qml/components/mediaplayer/mediacomponents/MediaPlayerLoadingBar.qml \
    qml/components/ConnectionStatus.qml \
    qml/pages/AboutPage.qml \
    qml/components/InfoLabel.qml \
    qml/pages/secretconversations/CreateSecretConversationPage.qml \
    qml/pages/secretconversations/SecretConversationPage.qml \
    qml/items/secretconversation/SecretConversationItem.qml \
    qml/components/TelegramBackground.qml \
    qml/items/secretconversation/SecretChatDiscarded.qml \
    qml/items/secretconversation/SecretChatWaiting.qml \
    qml/items/messageitem/MessageStatus.qml \
    qml/pages/settings/SettingsPage.qml \
    qml/pages/chat/ChangeChatTitle.qml \
    qml/items/messageitem/media/MessageAudio.qml \
    qml/items/messageitem/media/MessageVideo.qml

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/harbour-sailorgram-cs.ts \
                translations/harbour-sailorgram-da.ts \
                translations/harbour-sailorgram-de.ts \
                translations/harbour-sailorgram-el.ts \
                translations/harbour-sailorgram-es.ts \
                translations/harbour-sailorgram-fa_IR.ts \
                translations/harbour-sailorgram-fi.ts \
                translations/harbour-sailorgram-fr.ts \
                translations/harbour-sailorgram-it.ts \
                translations/harbour-sailorgram-nl_NL.ts \
                translations/harbour-sailorgram-ru.ts \
                translations/harbour-sailorgram.ts

HEADERS += \
    src/localstorage/telegramlocalstorage.h \
    src/telegramcalendar.h \
    src/dbus/notifications/notifications.h \
    src/sailorgram.h \
    src/filepicker/folderlistmodel.h \
    src/heartbeat.h \
    src/dbus/screenblank.h

RESOURCES += \
    resources.qrc

