import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../message/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/ColorScheme.js" as ColorScheme

Item
{
    property Context context
    property Message message

    signal closeRequested()

    id: dialogreplypreview
    height: column.height
    visible: message !== null

    Column
    {
        id: column
        height: lbluser.contentHeight + mtctextcontent.contentHeight

        anchors {
            left: parent.left
            top: parent.top
            right: btnclose.left
            leftMargin: Theme.paddingSmall
            rightMargin: Theme.paddingSmall
            topMargin: Theme.paddingSmall
        }

        Label
        {
            id: lbluser
            width: parent.width
            color: Theme.primaryColor
            font.bold: true
            font.pixelSize: Theme.fontSizeExtraSmall
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter

            text: {
                if(!message)
                    return "";

                var destname = message.out ? qsTr("You") : TelegramHelper.completeName(context.telegram.user(message.fromId));
                return qsTr("Reply to: %1").arg(destname);
            }
        }

        MessageTextContent
        {
            id: mtctextcontent
            font.pixelSize: Theme.fontSizeExtraSmall
            font.italic: true
            width: parent.width
            horizontalAlignment: Text.AlignLeft
            emojiPath: context.sailorgram.emojiPath
            rawText: message ? message.message : ""
            verticalAlignment: Text.AlignTop
            wrapMode: Text.Wrap
            elide: Text.ElideRight
            maximumLineCount: 3
            visible: text.length > 0
            color: Theme.primaryColor
            linkColor: message ? ColorScheme.colorizeLink(message, context) : Theme.highlightColor
        }
    }

    IconButton
    {
        id: btnclose
        width: parent.height
        anchors { top: parent.top; right: parent.right; bottom: parent.bottom; rightMargin: Theme.paddingSmall }
        icon.source: "image://theme/icon-s-clear-opaque-cross"

        onClicked: {
            dialogreplypreview.message = null;
            closeRequested();
        }
    }
}

