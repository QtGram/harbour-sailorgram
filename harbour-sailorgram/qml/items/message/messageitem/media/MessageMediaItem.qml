import QtQuick 2.1
import harbour.sailorgram.Telegram 1.0
import "../../../../models"

Item
{
    property Context context
    property SailorgramMessageItem sgMessageItem
    property real maxWidth
    property real contentWidth
    property real contentHeight

    id: messagemediaitem
    visible: sgMessageItem.isMediaMessage
    width: Math.min(contentWidth, maxWidth)
    height: contentHeight
}
