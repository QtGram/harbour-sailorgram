import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../../models"
import "../../../../components"
import "../../../../items/peer"
import "../../../../components/message"
import "../../../../js/ColorScheme.js" as ColorScheme
import "../../../../js/TelegramHelper.js" as TelegramHelper

MessageMediaItem
{
    readonly property User user: {
        var userid = message.media.userId;
        if(userid > 0)
            return context.telegram.user(userid);
        else
            return null;
    }

    id: contactdocument
    contentWidth: image.width + Math.max(name.calculatedWidth, phone.calculatedWidth) + Theme.paddingMedium
    contentHeight: row.height

    Component.onCompleted: {
        if(user)
            fileHandler.download()
    }

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
            source: user ? fileHandler.filePath : fileHandler.thumbPath;

            Rectangle
            {
                anchors.fill: parent
                color: Theme.highlightDimmerColor
                radius: image.width * 0.5
                visible: !fileHandler.downloaded && user

                Label
                {
                    anchors.centerIn: parent
                    font.bold: true
                    font.pixelSize: parent.height * 0.6
                    visible: parent.visible
                    text: {
                        if(user)
                            return TelegramHelper.fallbackText(null, user);
                        else
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
                text: {
                    var media = message.media;
                    if(media.firstName && media.lastName)
                        return media.firstName + " " + media.lastName;
                    return media.firstName + media.lastName;
                }
                color: ColorScheme.colorizeText(message, context)
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }

            ResizableLabel
            {
                id: phone
                width: parent.width - Theme.paddingSmall
                color: ColorScheme.colorizeLink(message, context)
                font.pixelSize: Theme.fontSizeExtraSmall
                text: message.media.phoneNumber
                wrapMode: Text.NoWrap
                elide: Text.ElideRight
            }
        }
    }
}
