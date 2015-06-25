import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import harbour.sailorgram.TelegramCalendar 1.0
import "media"

ListItem
{
    readonly property real typeMessageMediaDocument: 0x2fda2204
    readonly property real typeMessageMediaContact: 0x5e7d2f39
    readonly property real typeMessageMediaEmpty: 0x3ded6320
    readonly property real typeMessageMediaVideo: 0xa2d24290
    readonly property real typeMessageMediaUnsupported: 0x9f84f49e
    readonly property real typeMessageMediaAudio: 0xc6b68300
    readonly property real typeMessageMediaPhoto: 0xc8c45a2a
    readonly property real typeMessageMediaGeo: 0x56e0d474

    property Telegram telegram
    property Message message
    property bool me: (message.fromId === telegram.me)

    id: messageitem
    contentWidth: parent.width
    contentHeight: content.height

    Column
    {
        id: content
        anchors { top: parent.top; left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }

        height: {
            var h = messagetext.height;

            if(messagedocument.visible)
                h += messagedocument.height

            if(messagephoto.visible)
                h += messagephoto.height;

            return h;
        }

        MessageDocument
        {
            id: messagedocument
            telegram: messageitem.telegram
            message: messageitem.message
            me: messageitem.me
        }

        MessagePhoto
        {
            id: messagephoto
            telegram: messageitem.telegram
            message: messageitem.message
            me: messageitem.me
        }

        MessageText
        {
            id: messagetext
            width: parent.width
            message: messageitem.message
            me: messageitem.me
        }
    }
}
