import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../custom"
import "../preview"

Item
{
    property alias previewFrom: mediamessageitem.previewFrom
    property alias previewText: mediamessageitem.previewText
    property alias previewMessage: mediamessageitem.message

    id: messageinputpreview
    height: mediamessageitem.height
    visible: previewMessage !== null

    signal inputCancelled()

    BackgroundRectangle { anchors.fill: parent }

    Row
    {
        id: row
        anchors { fill: parent; leftMargin: Theme.paddingSmall }
        spacing: Theme.paddingSmall

        MessagePreviewItem
        {
            id: mediamessageitem
            width: parent.width - btnclose.width
        }

        IconButton
        {
            id: btnclose
            width: parent.height
            height: parent.height
            icon.source: "image://theme/icon-m-clear"
            onClicked: inputCancelled()
        }
    }
}
