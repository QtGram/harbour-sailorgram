import QtQuick 2.1
import Sailfish.Silica 1.0

Item
{
    property string waitingUser

    id: secretdialogwaiting
    width: busyindicator.width + lblwaiting.contentWidth + Theme.paddingMedium
    height: Theme.itemSizeSmall

    BusyIndicator
    {
        id: busyindicator
        size: BusyIndicatorSize.Small
        anchors { left: parent.left; verticalCenter: lblwaiting.verticalCenter }
        running: secretdialogwaiting.visible
    }

    Label
    {
        id: lblwaiting
        text: qsTr("Waiting for %1 to get online").arg(waitingUser)
        anchors { left: busyindicator.right; top: parent.top; bottom: parent.bottom; leftMargin: Theme.paddingMedium }
        color: Theme.secondaryHighlightColor
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        wrapMode: Text.NoWrap
        elide: Text.ElideRight
    }
}
