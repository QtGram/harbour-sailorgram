import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
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
        visible: messageType === Enums.TypeVenueMessage
        height: visible ? undefined : 0
        color: ColorScheme.colorizeText(messageType, messageOut, context)
        linkColor: ColorScheme.colorizeLink(messageType, messageOut, context)

        rawText: {
            return "";
            //FIXME: return message.media.venueTitle + "\n" +
            //message.media.venueAddress;
        }

        horizontalAlignment: {
            if(!messageOut)
                return Text.AlignRight;

            return Text.AlignLeft;
        }
    }

    MessageThumbnail
    {
        id: mapthumbnail
        cache: true
        anchors { left: parent.left; right: parent.right; top: mtctextcontent.bottom; bottom: parent.bottom }
        source: context.locationThumbnail(mediaItem.geo.lat, mediaItem.geo.longValue, width, height, 14)
    }
}
