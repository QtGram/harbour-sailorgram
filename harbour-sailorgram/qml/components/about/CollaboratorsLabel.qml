import QtQuick 2.1
import Sailfish.Silica 1.0

Column
{
    property alias title: typelabel.text
    property var labelData

    id: collaboratorslabel
    anchors { left: parent.left; right: parent.right }
    visible: labelData.length > 0

    Label
    {
        id: typelabel
        anchors { left: parent.left; right: parent.right }
        horizontalAlignment: Text.AlignHCenter
    }

    Repeater
    {
        model: labelData

        delegate: Label {
            anchors { left: parent.left; right: parent.right }
            horizontalAlignment: Text.AlignHCenter
            text: model.modelData
            font.pixelSize: Theme.fontSizeSmall
            wrapMode: Text.WordWrap
            color: Theme.secondaryColor
        }
    }
}
