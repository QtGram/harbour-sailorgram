import QtQuick 2.1
import harbour.sailorgram.TelegramQml 2.0
import "../../../models"
import "../../../js/TelegramConstants.js" as TelegramConstants
import "../../../js/TelegramHelper.js" as TelegramHelper
import "../../../js/ColorScheme.js" as ColorScheme

Item
{
    property Context context
    property var messageType
    property bool messageOut

    id: messagebubble
    layer.enabled: (context.bubbleshidden || !context.angledbubbles) ? false : true
    opacity: context.bubblesopacity / 100.0
    visible: !context.bubbleshidden

    Rectangle
    {
        id: mainbubble
        color: ColorScheme.colorizeBubble(messagebubble.messageType, messagebubble.messageOut, context)
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
            top: messageOut ? undefined : parent.top
            right: messageOut ? undefined : parent.right
            bottom: !messageOut ? undefined: parent.bottom
            left: !messageOut ? undefined : parent.left
        }
    }
}
