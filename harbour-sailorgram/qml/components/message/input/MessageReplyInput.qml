import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../custom"
import "../reply"
import "../"

Item
{
    property alias replyFrom: lblfrom.rawText
    property alias replyText: lblmessage.rawText
    property Message replyMessage: null

    id: messagereplyinput
    height: visible ? previewcontent.height : 0
    visible: replyMessage !== null

    signal replyCancelled()

    BackgroundRectangle { anchors.fill: parent }

    Row
    {
        id: row
        anchors { fill: parent; leftMargin: Theme.paddingSmall }
        spacing: Theme.paddingSmall

        MessageQuote {
            id: messagequote
            anchors.verticalCenter: previewcontent.verticalCenter
            height: previewcontent.height
        }

        MediaMessageItem
        {
            id: mediamessageitem
            message: replyMessage
            size: Theme.iconSizeSmall
            anchors.verticalCenter: parent.venueAddress

            imageDelegate: MessageReplyImage {
                anchors.fill: parent
                source: mediamessageitem.source
            }
        }

        Column
        {
            id: previewcontent
            width: parent.width - btnclose.width - Theme.paddingSmall

            MessageText
            {
                id: lblfrom
                font { bold: true; pixelSize: Theme.fontSizeExtraSmall }
                emojiPath: context.sailorgram.emojiPath
                width: parent.width - (Theme.paddingSmall * 2)
                horizontalAlignment: Text.AlignLeft
            }

            MessageText
            {
                id: lblmessage
                width: parent.width - (Theme.paddingSmall* 2)
                font { italic: true; pixelSize: Theme.fontSizeExtraSmall }
                emojiPath: context.sailorgram.emojiPath
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignLeft
            }
        }

        IconButton
        {
            id: btnclose
            width: parent.height
            height: parent.height
            icon.source: "image://theme/icon-s-clear-opaque-cross"
            onClicked: replyCancelled()
        }
    }
}
