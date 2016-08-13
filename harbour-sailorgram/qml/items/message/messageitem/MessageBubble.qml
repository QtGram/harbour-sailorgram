import QtQuick 2.1
import harbour.sailorgram.Telegram 1.0
import "../../../models"
import "../../../js/ColorScheme.js" as ColorScheme

Item
{
    property Context context
    property SailorgramMessageItem sgMessageItem

    id: messagebubble
    layer.enabled: (context.bubbleshidden || !context.angledbubbles) ? false : true
    opacity: context.bubblesopacity / 100.0
    visible: !context.bubbleshidden

    Rectangle
    {
        id: mainbubble
        color: ColorScheme.colorizeBubble(sgMessageItem.isActionMessage, sgMessageItem.isMessageOut, context)
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
            top: sgMessageItem.isMessageOut ? undefined : parent.top
            right: sgMessageItem.isMessageOut ? undefined : parent.right
            bottom: !sgMessageItem.isMessageOut ? undefined: parent.bottom
            left: !sgMessageItem.isMessageOut ? undefined : parent.left
        }
    }
}
