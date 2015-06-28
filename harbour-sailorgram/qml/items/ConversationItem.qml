import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import harbour.sailorgram.TelegramCalendar 1.0
import "../items/user"
import "../js/TelegramHelper.js" as TelegramHelper

Item
{
    readonly property real typeMessageMediaDocument: 0x2fda2204
    readonly property real typeMessageMediaContact: 0x5e7d2f39
    readonly property real typeMessageMediaEmpty: 0x3ded6320
    readonly property real typeMessageMediaVideo: 0xa2d24290
    readonly property real typeMessageMediaUnsupported: 0x9f84f49e
    readonly property real typeMessageMediaAudio: 0xc6b68300
    readonly property real typeMessageMediaPhoto: 0xc8c45a2a
    readonly property real typeMessageMediaGeo: 0x56e0d474

    property Dialog dialog
    property User user
    property Message message

    onDialogChanged: {
        user = telegram.user(dialog.peer.userId);
        message = telegram.message(dialog.topMessage);
    }

    id: conversationitem

    Connections
    {
        target: dialog

        onTopMessageChanged: {
            message = telegram.message(dialog.topMessage);
        }
    }

    Row
    {
        anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
        spacing: Theme.paddingSmall

        UserAvatar
        {
            id: useravatar
            width: conversationitem.height
            height: conversationitem.height
            telegram: conversationspage.telegram
            user: conversationitem.user
        }

        Column
        {
            width: parent.width - useravatar.width
            anchors { top: parent.top; bottom: parent.bottom }

            Row
            {
                height: conversationitem.height / 2
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }

                Label {
                    id: lbluser
                    width: parent.width - lbltime.width
                    text: TelegramHelper.userName(user)
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height
                    color: Theme.highlightColor
                }

                Label {
                    id: lbltime
                    height: parent.height
                    font.pixelSize: Theme.fontSizeTiny
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                    text: TelegramCalendar.timeToString(message.date)
                }
            }

            Row
            {
                height: conversationitem.height / 2
                anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingMedium }

                Label
                {
                    id: lbllastmessage
                    width: parent.width - rectunread.width
                    height: parent.height
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Theme.fontSizeExtraSmall

                    text: {
                        if(message.media) {
                            switch(message.media.classType) {
                                case typeMessageMediaAudio:
                                    return qsTr("Audio");

                                case typeMessageMediaContact:
                                    return qsTr("Contact");

                                case typeMessageMediaDocument:
                                    return qsTr("Document");

                                case typeMessageMediaGeo:
                                    return qsTr("Geo");

                                case typeMessageMediaPhoto:
                                    return qsTr("Photo");

                                case typeMessageMediaUnsupported:
                                    return qsTr("Unsupported Media");

                                case typeMessageMediaVideo:
                                    return qsTr("Video");

                                default:
                                    break;
                            }
                        }

                        return message.message
                    }
                }

                Rectangle
                {
                    id: rectunread
                    width: parent.height
                    height: parent.height
                    color: Theme.secondaryHighlightColor
                    visible: dialog.unreadCount > 0
                    radius: width * 0.5

                    Label
                    {
                        anchors.centerIn: parent
                        font.pixelSize: Theme.fontSizeTiny
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.bold: true
                        text: dialog.unreadCount
                    }
                }
            }
        }
    }
}
