import QtQuick 2.1
import harbour.sailorgram.TelegramQml 1.0
import "../../../models"
import "../../../js/TelegramConstants.js" as TelegramConstants
import "../../../js/TelegramHelper.js" as TelegramHelper
import "../../../js/ColorScheme.js" as ColorScheme

Item
{
    property Context context
    property Message message

    id: messagebubble
    layer.enabled: context.bubbleshidden ? false : true
    opacity: context.bubblesopacity / 100.0

    visible: {
        var bubblevisible = !context.bubbleshidden;

        if(bubblevisible)
        {
            if(!TelegramHelper.isMediaMessage(message))
                return bubblevisible;

            if(message.media.classType !== TelegramConstants.typeMessageMediaDocument)
                return bubblevisible;

            if(context.telegram.documentIsSticker(message.media.document))
                return false;
        }

        return bubblevisible;
    }

    Rectangle
    {
        id: mainbubble
        color: ColorScheme.colorizeBubble(message, context)
        anchors.fill: parent
        radius: 20
        smooth: true
    }

    Rectangle
    {
        id: flatangle
        width: mainbubble.radius * 2
        height: mainbubble.radius * 2
        color: mainbubble.color

        anchors {
            top: parent.top;
            left: message.out ? parent.left : undefined
            right: message.out ? undefined : parent.right
        }
    }
}
