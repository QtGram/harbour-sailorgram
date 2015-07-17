import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../models"

Dialog
{
    property var user

    id: dlgchangeusername
    allowedOrientations: defaultAllowedOrientations
    acceptDestinationAction: PageStackAction.Pop
    canAccept: ((tfusername.text.length >= 5) && (tfusername.text.length <= 32) && /[a-zA-Z0-9_]+/.test(tfusername.text)) || !tfusername.text.length

    onAccepted: {
        user.username = tfusername.text;
    }

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        Column
        {
            id: content
            width: parent.width
            spacing: Theme.paddingMedium

            DialogHeader
            {
                acceptText: qsTr("Change")
            }

            TextField
            {
                id: tfusername
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                placeholderText: qsTr("Username")
                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
            }

            Label
            {
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignTop
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.WordWrap
                text: qsTr("You can choose an username on Telegram. If you do, other people may contact you without knowing your phone number.\n\nYou can use a-z, 0-9 and underscores.\nMinimum length is 5 characters.")
            }
        }
    }
}
