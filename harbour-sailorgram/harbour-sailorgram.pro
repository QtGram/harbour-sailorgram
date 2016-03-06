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

include($$PWD/../config.pri)

CONFIG += sailfishapp
QT     += sql dbus multimedia

LIBS += -L$$LIBQTELEGRAM_LIB_DIR -L$$TELEGRAMQML_LIB_DIR -lqtelegram-ae -ltelegramqml
INCLUDEPATH += $$LIBQTELEGRAM_INCLUDE_PATH $$TELEGRAMQML_INCLUDE_PATH

# WebP Plugin
webp.files = $$OUT_PWD/../webp-plugin/plugins/imageformats/*.so
webp.path  = /usr/share/$$TARGET/lib/imageformats
INSTALLS += webp

# Telegram Public Key
server_pub.files = $$PWD/server.pub
server_pub.path  = /usr/share/$$TARGET
INSTALLS    += server_pub
OTHER_FILES += $$server_pub.files

# Emoji Set
emoji.files = $$PWD/res/emoji/*.png
emoji.path  = /usr/share/$$TARGET/emoji
INSTALLS += emoji

# Sticker Set
#stickers.files = $$PWD/res/stickers/telegram/*.webp
#stickers.path = /usr/share/$$TARGET/stickers/telegram
#INSTALLS += stickers

# Lipstick Config
lipstick_config.files = $$PWD/lipstick/*.conf
lipstick_config.path  = /usr/share/lipstick/notificationcategories
INSTALLS    += lipstick_config
OTHER_FILES += $$lipstick_config.files

# DBus Config
dbus_config.files = $$PWD/dbus/*.service
dbus_config.path  = /usr/share/dbus-1/services
INSTALLS    += dbus_config
OTHER_FILES += $$dbus_config.files

# Systemd Config
systemd.files = $$PWD/systemd/*.service
systemd.path  = /usr/lib/systemd/user
INSTALLS    += systemd
OTHER_FILES += $$systemd.files

# Events Config
events.files = $$PWD/events/*.ini
events.path  = /usr/share/ngfd/events.d
INSTALLS    += events
OTHER_FILES += $$events.files

# MCE Config
mce.files = $$PWD/mce/*.ini
mce.path  = /etc/mce
INSTALLS    += mce
OTHER_FILES += $$mce.files

# Headers
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
    src/model/sailorgramcontactsmodel.h \
    src/item/translationinfoitem.h

# Sources
SOURCES += \
    src/harbour-sailorgram.cpp \
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
    src/model/sailorgramcontactsmodel.cpp \
    src/item/translationinfoitem.cpp

# QML
OTHER_FILES += \
    qml/js/*.js \
    qml/harbour-sailorgram.qml \
    qml/cover/CoverPage.qml \
    qml/components/ClickableLabel.qml \
    qml/components/CollaboratorsLabel.qml \
    qml/components/ConnectingLabel.qml \
    qml/components/ConnectionStatus.qml \
    qml/components/cover/CoverLabel.qml \
    qml/components/DebugLabel.qml \
    qml/components/mediaplayer/mediacomponents/MediaPlayerCursor.qml \
    qml/components/mediaplayer/mediacomponents/MediaPlayerLoadingBar.qml \
    qml/components/mediaplayer/mediacomponents/MediaPlayerProgressBar.qml \
    qml/components/mediaplayer/mediacomponents/MediaPlayerTimings.qml \
    qml/components/mediaplayer/mediacomponents/MediaPlayerTitle.qml \
    qml/components/mediaplayer/mediacomponents/MediaPlayerToolBar.qml \
    qml/components/mediaplayer/MediaPlayer.qml \
    qml/components/message/MessageContainer.qml \
    qml/components/message/MessageMediaProgressBar.qml \
    qml/components/message/MessageTypesPool.qml \
    qml/components/PopupMessage.qml \
    qml/components/ResizableLabel.qml \
    qml/components/RoundedImage.qml \
    qml/components/search/MessageSearchList.qml \
    qml/components/SettingLabel.qml \
    qml/components/telegram/TelegramBackground.qml \
    qml/components/telegram/TelegramMenuItem.qml \
    qml/components/telegram/TelegramPullDownMenu.qml \
    qml/components/ThirdPartyLabel.qml \
    qml/items/chat/ChatInfo.qml \
    qml/items/dialog/DialogItem.qml \
    qml/items/dialog/DialogReplyPreview.qml \
    qml/items/dialog/DialogTextInput.qml \
    qml/items/message/forward/ForwardContactItem.qml \
    qml/items/message/forward/ForwardDialogItem.qml \
    qml/items/message/messageitem/media/MessageAudio.qml \
    qml/items/message/messageitem/media/MessageContact.qml \
    qml/items/message/messageitem/media/MessageDocument.qml \
    qml/items/message/messageitem/media/MessageLocation.qml \
    qml/items/message/messageitem/media/MessageMediaItem.qml \
    qml/items/message/messageitem/media/MessagePhoto.qml \
    qml/items/message/messageitem/media/MessageSticker.qml \
    qml/items/message/messageitem/media/MessageThumbnail.qml \
    qml/items/message/messageitem/media/MessageVideo.qml \
    qml/items/message/messageitem/media/MessageWebPage.qml \
    qml/items/message/messageitem/MessageBubble.qml \
    qml/items/message/messageitem/MessageItem.qml \
    qml/items/message/messageitem/MessagePreview.qml \
    qml/items/message/messageitem/MessageStatus.qml \
    qml/items/message/messageitem/MessageTextContent.qml \
    qml/items/message/messageitem/MessageText.qml \
    qml/items/message/MessageView.qml \
    qml/items/peer/PeerImage.qml \
    qml/items/peer/PeerItem.qml \
    qml/items/peer/PeerProfile.qml \
    qml/items/search/MessageSearchItem.qml \
    qml/items/secretdialog/SecretDialogDiscarded.qml \
    qml/items/secretdialog/SecretDialogWaiting.qml \
    qml/items/sticker/StickerItem.qml \
    qml/items/sticker/StickerSetImage.qml \
    qml/items/user/UserInfo.qml \
    qml/items/user/UserItem.qml \
    qml/menus/chat/ChatInfoMenu.qml \
    qml/menus/DialogItemMenu.qml \
    qml/menus/MessageMenu.qml \
    qml/menus/UserMenu.qml \
    qml/models/Context.qml \
    qml/models/ErrorsModel.qml \
    qml/pages/about/AboutPage.qml \
    qml/pages/about/DevelopersPage.qml \
    qml/pages/about/ThirdPartyPage.qml \
    qml/pages/about/TranslationsPage.qml \
    qml/pages/chat/AddContactsPage.qml \
    qml/pages/chat/ChangeChatTitle.qml \
    qml/pages/chat/CreateChatPage.qml \
    qml/pages/contacts/AddContactPage.qml \
    qml/pages/contacts/ContactPage.qml \
    qml/pages/contacts/ContactsPage.qml \
    qml/pages/dialogs/DialogInfoPage.qml \
    qml/pages/dialogs/DialogPage.qml \
    qml/pages/dialogs/DialogsPage.qml \
    qml/pages/dialogs/forward/ForwardContactPage.qml \
    qml/pages/dialogs/forward/ForwardDialogPage.qml \
    qml/pages/login/AuthorizationPage.qml \
    qml/pages/login/ConnectionPage.qml \
    qml/pages/login/PhoneNumberPage.qml \
    qml/pages/login/SignUpPage.qml \
    qml/pages/media/MediaPage.qml \
    qml/pages/media/MediaPhotoPage.qml \
    qml/pages/media/MediaPlayerPage.qml \
    qml/pages/profile/ChangeUsernamePage.qml \
    qml/pages/profile/ProfilePage.qml \
    qml/pages/secretdialogs/CreateSecretDialogPage.qml \
    qml/pages/selector/SelectorFilesPage.qml \
    qml/pages/selector/SelectorImagesPage.qml \
    qml/pages/selector/SelectorLocationPage.qml \
    qml/pages/selector/SelectorMainPage.qml \
    qml/pages/selector/SelectorPhotoCapturePage.qml \
    qml/pages/selector/SelectorSoundRecordPage.qml \
    qml/pages/selector/SelectorStickerPage.qml \
    qml/pages/settings/ChatSettingsPage.qml \
    qml/pages/settings/DaemonSettingsPage.qml \
    qml/pages/settings/DebugSettingsPage.qml \
    qml/pages/settings/SettingsPage.qml

# Translations
CONFIG += sailfishapp_i18n

OTHER_FILES += \
    translations/*.ts \
    update_translations_json.sh

TRANSLATIONS += \
    translations/harbour-sailorgram-be.ts \
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

# Resources
RESOURCES += \
    resources.qrc

# Packaging
SAILFISHAPP_ICONS = \
    86x86 \
    108x108 \
    128x128

OTHER_FILES += \
    rpm/harbour-sailorgram.changes \
    rpm/harbour-sailorgram.spec \
    rpm/harbour-sailorgram.yaml \
    harbour-sailorgram.desktop \
    qml/cover/*.png
