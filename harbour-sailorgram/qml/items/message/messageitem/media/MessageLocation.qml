import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0
import "../"
import "../../../../js/ColorScheme.js" as ColorScheme
import "../../../../js/TelegramConstants.js" as TelegramConstants

MessageMediaItem
{
    id: messagelocation
    contentWidth: maxWidth
    contentHeight: maxWidth

    MessageTextContent
    {
        id: mtctextcontent
        width: parent.width
        verticalAlignment: Text.AlignTop
        font.pixelSize: Theme.fontSizeSmall
        emojiPath: context.sailorgram.emojiPath
        wrapMode: Text.Wrap
        visible: sgMessageItem.messageType === SailorgramEnums.MessageTypeVenue
        height: visible ? undefined : 0
        color: ColorScheme.colorizeText(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
        linkColor: ColorScheme.colorizeLink(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
        rawText: sgMessageItem.messageMedia.geoPoint.address

        horizontalAlignment: {
            if(!sgMessageItem.isMessageOut)
                return Text.AlignRight;

            return Text.AlignLeft;
        }
    }

    MessageThumbnail
    {
        id: mapthumbnail
        anchors { left: parent.left; right: parent.right; top: mtctextcontent.bottom; bottom: parent.bottom }
        source: messagelocation.context.locationThumbnail(sgMessageItem.messageMedia.geoPoint.latitude, sgMessageItem.messageMedia.geoPoint.longitude, width, height, 14)
    }
}
