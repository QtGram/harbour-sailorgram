import QtQuick 2.1
import harbour.sailorgram.TelegramQml 2.0
import "../../../models"
import "../../../js/TelegramConstants.js" as TelegramConstants
import "../../../js/TelegramHelper.js" as TelegramHelper
import "../../../js/ColorScheme.js" as ColorScheme

Item
{
    property Context context
    property var modelItem

    id: messagebubble
    layer.enabled: (context.bubbleshidden || !context.angledbubbles) ? false : true
    opacity: context.bubblesopacity / 100.0

    visible: {
        var bubblevisible = !context.bubbleshidden;

        /* FIXME:
        if(bubblevisible)
        {
            if(!TelegramHelper.isMediaMessage(message))
                return bubblevisible;

            if(message.media.classType !== TelegramConstants.typeMessageMediaDocument)
                return bubblevisible;
        }
        */

        return bubblevisible;
    }

    Rectangle
    {
        id: mainbubble
        color: ColorScheme.colorizeBubble(modelItem, context)
        anchors.fill: parent
        radius: context.bubbleradius
        smooth: true
    }

    Rectangle
    {
        id: flatangle
        width: mainbubble.radius * 2
        height: mainbubble.radius * 2
        color: mainbubble.color
        visible: context.angledbubbles

        anchors {
            top: modelItem.out ? undefined : parent.top
            right: modelItem.out ? undefined : parent.right
            bottom: !modelItem.out ? undefined: parent.bottom
            left: !modelItem.out ? undefined : parent.left
        }
    }
}
