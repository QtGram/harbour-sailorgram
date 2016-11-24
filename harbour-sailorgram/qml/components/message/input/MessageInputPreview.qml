import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../custom"
import "../reply"
import "../"

Item
{
    property alias previewFrom: lblfrom.rawText
    property alias previewText: lblmessage.rawText
    property Message previewMessage: null

    id: messageinputpreview
    height: Theme.itemSizeSmall
    visible: previewMessage !== null

    signal inputCancelled()

    BackgroundRectangle { anchors.fill: parent }

    Row
    {
        id: row
        anchors { fill: parent; leftMargin: Theme.paddingSmall }
        spacing: Theme.paddingSmall

        MessageQuote {
            id: messagequote
            anchors.verticalCenter: parent.verticalCenter
            height: previewcontent.height
        }

        MediaMessageItem
        {
            id: mediamessageitem
            message: previewMessage
            size: Theme.itemSizeSmall
            anchors.verticalCenter: parent.verticalCenter

            imageDelegate: MessageReplyImage {
                anchors.fill: parent
                source: mediamessageitem.source
            }
        }

        Column
        {
            id: previewcontent
            anchors.verticalCenter: parent.verticalCenter
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
            onClicked: inputCancelled()
        }
    }
}
