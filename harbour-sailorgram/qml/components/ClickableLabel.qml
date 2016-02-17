import QtQuick 2.1
import Sailfish.Silica 1.0

Item
{
    property alias labelText: lbltext.text
    property alias labelFont: lbltext.font
    property alias labelVerticalAlignment: lbltext.verticalAlignment
    property alias labelHorizontalAlignment: lbltext.horizontalAlignment
    property bool remorseRequired: false
    property string remorseMessage

    signal actionRequested()

    BackgroundItem
    {
        id: backgrounditem
        anchors.fill: parent

        RemorseItem { id: remorseitem }

        Label
        {
            id: lbltext
            anchors { fill: parent; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
            verticalAlignment: Text.AlignVCenter
        }

        onClicked: {
            if(!remorseRequired) {
                actionRequested();
                return;
            }

            remorseitem.execute(backgrounditem, remorseMessage,
                                    function() {
                                        actionRequested();
                                    });
        }
    }
}
