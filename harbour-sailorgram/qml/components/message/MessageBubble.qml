import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../model"
import "../../js/ColorScheme.js" as ColorScheme

Item
{
    id: messagebubble
    layer.enabled: (context.bubbleshidden || !context.angledbubbles) ? false : true
    opacity: context.bubblesopacity / 100.0

    Rectangle
    {
        id: mainbubble
        color: messagemodelitem.bubbleColor
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
            top: model.isMessageOut ? undefined : parent.top
            right: !model.isMessageOut ? undefined : parent.right
            bottom: !model.isMessageOut ? undefined: parent.bottom
            left: model.isMessageOut ? undefined : parent.left
        }
    }
}
