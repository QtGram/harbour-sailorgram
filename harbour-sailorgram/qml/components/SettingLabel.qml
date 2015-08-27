import QtQuick 2.1
import Sailfish.Silica 1.0

Item
{
    property alias icon: imgicon.source
    property alias text: lbltext.text
    property bool remorseRequired: false
    property string actionMessage

    signal actionRequested()

    BackgroundItem
    {
        id: backgrounditem
        anchors.fill: parent

        RemorseItem { id: remorseitem }

        Row
        {
            id: row
            spacing: Theme.paddingSmall
            height: parent.height
            anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }

            Image
            {
                id: imgicon
                width: lbltext.contentHeight
                height: lbltext.contentHeight
                anchors.verticalCenter: lbltext.verticalCenter
                fillMode: Image.PreserveAspectFit
                visible: imgicon.status === Image.Ready
            }

            Label
            {
                id: lbltext
                width: parent.width - imgicon.width
                height: parent.height
                verticalAlignment: Text.AlignVCenter
            }
        }

        onClicked: {
            if(!remorseRequired) {
                actionRequested();
                return;
            }

            remorseitem.execute(backgrounditem, actionMessage,
                                function() {
                                    actionRequested();
                                });
        }
    }
}
