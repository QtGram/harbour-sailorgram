import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../../../models"
import "../../../../components"
import "../../../../items/peer"
import "../../../../components/message"
import "../../../../js/ColorScheme.js" as ColorScheme
import "../../../../js/TelegramHelper.js" as TelegramHelper

MessageMediaItem
{
    id: contactdocument
    contentWidth: image.width + Math.max(name.calculatedWidth, phone.calculatedWidth) + Theme.paddingMedium
    contentHeight: row.height

    Row
    {
        id: row
        width: parent.width
        height: Math.max(info.height, image.height)
        spacing: Theme.paddingMedium

        RoundedImage
        {
            id: image
            anchors.verticalCenter: info.verticalCenter
            height: Theme.iconSizeMedium
            width: Theme.iconSizeMedium
            //image: user ? fileHandler.filePath : fileHandler.thumbPath;

            Rectangle
            {
                anchors.fill: parent
                color: Theme.highlightDimmerColor
                radius: image.width * 0.5
                visible: !sgMessageItem.messageMedia.isTransfered

                Label
                {
                    anchors.centerIn: parent
                    font.bold: true
                    font.pixelSize: parent.height * 0.6
                    visible: parent.visible
                    text: {
                        /* FIXME:
                        if(user)
                            return TelegramHelper.fallbackText(null, user);
                        else
                        */
                            return "";
                    }
                }
            }
        }

        Column
        {
            id: info
            width: parent.width - image.width

            ResizableLabel
            {
                id: name
                width: parent.width - Theme.paddingSmall
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: Theme.fontSizeSmall

                /* FIXME:
                text: {
                    var media = message.media;
                    if(media.firstName && media.lastName)
                        return media.firstName + " " + media.lastName;
                    return media.firstName + media.lastName;
                }
                */

                color: ColorScheme.colorizeText(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }

            ResizableLabel
            {
                id: phone
                width: parent.width - Theme.paddingSmall
                color: ColorScheme.colorizeLink(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
                font.pixelSize: Theme.fontSizeExtraSmall
                //FIXME: text: message.media.phoneNumber
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }
        }
    }
}
