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

CONFIG += \
    sailfishapp \
    c++11

QT += sql dbus multimedia

include($$PWD/../config.pri)

LIBS += -L$$LIBQTELEGRAM_LIB_DIR -L$$TELEGRAMQML_LIB_DIR -lqtelegram-ae -ltelegramqml
INCLUDEPATH += $$LIBQTELEGRAM_INCLUDE_PATH $$TELEGRAMQML_INCLUDE_PATH

# WebP Plugin
webp.files = $$OUT_PWD/../webp-plugin/plugins/imageformats/*.so
webp.path = /usr/share/$$TARGET/lib/imageformats
INSTALLS += webp

# TelegramQml
libtelegram_qml.files = $$OUT_PWD/../TelegramQML/*.so*
libtelegram_qml.path = /usr/share/$$TARGET/lib
INSTALLS += libtelegram_qml

# LibQTelegram-ae
libqtelegram_ae.files = $$OUT_PWD/../libqtelegram-aseman-edition/*.so*
libqtelegram_ae.path = /usr/share/$$TARGET/lib
INSTALLS += libqtelegram_ae

# Telegram Public Key
server_pub.files = $$PWD/server.pub
server_pub.path = /usr/share/$$TARGET
INSTALLS += server_pub

# Emoji Set
emoji.files = $$PWD/res/emoji/*.png
emoji.path = /usr/share/$$TARGET/emoji
INSTALLS += emoji

# Sticker Set
#stickers.files = $$PWD/res/stickers/telegram/*.webp
#stickers.path = /usr/share/$$TARGET/stickers/telegram
#INSTALLS += stickers

# Lipstick Config
lipstick_config.files = $$PWD/lipstick/*.conf
lipstick_config.path = /usr/share/lipstick/notificationcategories
INSTALLS += lipstick_config

# DBus Config
dbus_config.files = $$PWD/dbus/*.service
dbus_config.path = /usr/share/dbus-1/services
INSTALLS += dbus_config

# Systemd Config
systemd.files = $$PWD/systemd/*.service
systemd.path = /usr/lib/systemd/user
INSTALLS += systemd

# Events Config
events.files = $$PWD/events/*.ini
events.path = /usr/share/ngfd/events.d
INSTALLS += events

# MCE Config
mce.files = $$PWD/mce/*.ini
mce.path = /etc/mce
INSTALLS += mce

SOURCES += src/harbour-sailorgram.cpp \
    src/sailorgram.cpp \
    src/dbus/screenblank.cpp \
    src/dbus/interface/sailorgramadaptor.cpp \
    src/dbus/interface/sailorgraminterface.cpp \
    src/dbus/notification/notification.cpp \
    src/dbus/notification/notificationmanagerproxy.cpp \
    src/selector/audiorecorder.cpp \
    src/selector/thumbnailprovider.cpp \
    src/selector/exif/exif.cpp \
    src/model/dialogscovermodel.cpp \
    src/selector/filesmodel.cpp \
    src/selector/filesmodelworker.cpp \
    src/dbus/connectivitychecker.cpp \
    src/model/sailorgramcontactsmodel.cpp

OTHER_FILES += qml/harbour-sailorgram.qml \
    rpm/harbour-sailorgram.changes.in \
    rpm/harbour-sailorgram.spec \
    rpm/harbour-sailorgram.yaml \
    translations/*.ts \
    translations/translations.json \
    lipstick/*.conf \
    harbour-sailorgram.desktop \
    qml/pages/login/ConnectionPage.qml \
    server.pub \
    qml/pages/login/PhoneNumberPage.qml \
    qml/pages/login/AuthorizationPage.qml \
    qml/js/Settings.js \
    qml/items/user/UserItem.qml \
    qml/js/CountryList.js \
    qml/items/message/messageitem/MessageItem.qml \
    qml/js/TelegramHelper.js \
    qml/items/message/messageitem/MessageText.qml \
    qml/items/message/messageitem/media/MessageMediaItem.qml \
    qml/items/message/messageitem/media/MessagePhoto.qml \
    qml/items/message/messageitem/media/MessageDocument.qml \
    qml/components/PopupMessage.qml \
    qml/menus/UserMenu.qml \
    qml/components/ClickableLabel.qml \
    qml/items/message/messageitem/media/MessageThumbnail.qml \
    qml/js/TelegramConstants.js \
    qml/pages/login/SignUpPage.qml \
    qml/items/peer/PeerItem.qml \
    qml/items/peer/PeerImage.qml \
    qml/items/user/UserInfo.qml \
    qml/items/chat/ChatInfo.qml \
    qml/js/TelegramAction.js \
    qml/pages/chat/AddContactsPage.qml \
    qml/menus/MessageMenu.qml \
    qml/pages/chat/CreateChatPage.qml \
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
    qml/items/message/messageitem/MessageStatus.qml \
    qml/pages/settings/SettingsPage.qml \
    qml/pages/chat/ChangeChatTitle.qml \
    qml/items/message/messageitem/media/MessageAudio.qml \
    qml/items/message/messageitem/media/MessageVideo.qml \
    qml/pages/media/MediaPage.qml \
    qml/components/SettingLabel.qml \
    qml/pages/settings/ChatSettingsPage.qml \
    qml/pages/settings/DebugSettingsPage.qml \
    qml/components/DebugLabel.qml \
    qml/models/ErrorsModel.qml

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/harbour-sailorgram-be.ts \
                translations/harbour-sailorgram-cs.ts \
                translations/harbour-sailorgram-da.ts \
                translations/harbour-sailorgram-de.ts \
                translations/harbour-sailorgram-de_DE.ts \
                translations/harbour-sailorgram-el.ts \
                translations/harbour-sailorgram-es.ts \
                translations/harbour-sailorgram-fa_IR.ts \
                translations/harbour-sailorgram-fi.ts \
                translations/harbour-sailorgram-fr.ts \
                translations/harbour-sailorgram-gl.ts \
                translations/harbour-sailorgram-it.ts \
                translations/harbour-sailorgram-nl_NL.ts \
                translations/harbour-sailorgram-ru.ts \
                translations/harbour-sailorgram-sv.ts \
                translations/harbour-sailorgram-uk_UA.ts \
                translations/harbour-sailorgram.ts

HEADERS += \
    src/sailorgram.h \
    src/dbus/screenblank.h \
    src/dbus/interface/sailorgramadaptor.h \
    src/dbus/interface/sailorgraminterface.h \
    src/dbus/notification/notification.h \
    src/dbus/notification/notificationmanagerproxy.h \
    src/selector/audiorecorder.h \
    src/selector/thumbnailprovider.h \
    src/selector/exif/exif.h \
    src/model/dialogscovermodel.h \
    src/selector/filesmodel.h \
    src/selector/filesmodelworker.h \
    src/dbus/connectivitychecker.h \
    src/model/sailorgramcontactsmodel.h

RESOURCES += \
    resources.qrc

DISTFILES += \
    qml/items/dialog/DialogItem.qml \
    qml/pages/dialogs/DialogsPage.qml \
    qml/items/secretdialog/SecretDialogDiscarded.qml \
    qml/pages/dialogs/DialogInfoPage.qml \
    qml/pages/dialogs/DialogPage.qml \
    qml/items/message/MessageView.qml \
    qml/components/telegram/TelegramBackground.qml \
    qml/items/dialog/DialogTextInput.qml \
    qml/pages/secretdialogs/CreateSecretDialogPage.qml \
    qml/items/secretdialog/SecretDialogWaiting.qml \
    qml/pages/contacts/ContactsPage.qml \
    qml/pages/contacts/ContactPage.qml \
    qml/pages/contacts/AddContactPage.qml \
    qml/js/TextElaborator.js \
    qml/items/message/messageitem/MessageTextContent.qml \
    qml/js/ColorScheme.js \
    qml/js/TelegramMedia.js \
    dbus/org.harbour.sailorgram.service \
    events/sailorgram.ini \
    lipstick/harbour.sailorgram.notificationfg.conf \
    events/sailorgram_fg.ini \
    systemd/harbour-sailorgram-notifications.service \
    qml/pages/settings/DaemonSettingsPage.qml \
    qml/js/Emoji.js \
    qml/pages/selector/SelectorFilesPage.qml \
    qml/pages/selector/SelectorImagesPage.qml \
    qml/pages/selector/SelectorLocationPage.qml \
    qml/pages/selector/SelectorMainPage.qml \
    qml/pages/selector/SelectorPhotoCapturePage.qml \
    qml/pages/selector/SelectorSoundRecordPage.qml \
    qml/pages/selector/SelectorStickerPage.qml \
    qml/items/message/messageitem/media/MessageSticker.qml \
    qml/items/sticker/StickerSetImage.qml \
    qml/items/sticker/StickerItem.qml \
    qml/items/message/messageitem/media/MessageLocation.qml \
    qml/components/message/MessageTypesPool.qml \
    qml/components/message/MessageContainer.qml \
    mce/10sailorgram-led.ini \
    qml/components/search/MessageSearchList.qml \
    qml/items/search/MessageSearchItem.qml \
    qml/items/message/messageitem/MessageBubble.qml \
    qml/components/RoundedImage.qml \
    qml/items/peer/PeerProfile.qml \
    qml/components/message/MessageMediaProgressBar.qml \
    qml/items/dialog/DialogReplyPreview.qml \
    qml/items/message/messageitem/MessagePreview.qml \
    qml/items/message/messageitem/media/MessageWebPage.qml \
    qml/components/ResizableLabel.qml \
    qml/menus/DialogItemMenu.qml \
    qml/pages/dialogs/forward/ForwardDialogPage.qml \
    qml/pages/dialogs/forward/ForwardContactPage.qml \
    qml/items/message/forward/ForwardDialogItem.qml \
    qml/items/message/forward/ForwardContactItem.qml
