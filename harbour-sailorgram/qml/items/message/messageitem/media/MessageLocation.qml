import QtQuick 2.1
import Sailfish.Silica 1.0
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
        visible: message.media.classType === TelegramConstants.typeMessageMediaVenue
        height: visible ? undefined : 0
        color: ColorScheme.colorizeText(message, context)
        linkColor: ColorScheme.colorizeLink(message, context)

        rawText: {
            return message.media.venueTitle + "\n" +
                   message.media.venueAddress;
        }

        horizontalAlignment: {
            if(!message.out)
                return Text.AlignRight;

            return Text.AlignLeft;
        }
    }

    MessageThumbnail
    {
        id: mapthumbnail
        cache: true
        anchors { left: parent.left; right: parent.right; top: mtctextcontent.bottom; bottom: parent.bottom }
        source: context.locationThumbnail(message.media.geo.lat, message.media.geo.longitude, width, height, 14);
    }
}
