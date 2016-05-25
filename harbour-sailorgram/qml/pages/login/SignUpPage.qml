import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../models"

Dialog
{
    property Context context

    id: dlgsignup
    allowedOrientations: defaultAllowedOrientations
    canAccept: tffirstname.text.length > 0

    acceptDestination: Component {
        ConnectionPage {
            context: dlgsignup.context
        }
    }

    onAccepted: context.authenticate.signUp(tffirstname.text, tfflastname.text)

    SilicaFlickable
    {
        id: flickable
        anchors.fill: parent
        contentHeight: content.height

        PullDownMenu
        {
            MenuItem
            {
                text: qsTr("Exit")
                visible: context.sailorgram.keepRunning
                onClicked: Qt.quit()
            }
        }

        Column
        {
            id: content
            width: parent.width
            spacing: Theme.paddingLarge

            DialogHeader {
                acceptText: qsTr("Sign Up")
            }

            Image
            {
                id: imgwaiting
                source: "qrc:///res/waiting.png"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label
            {
                id: lblinfo
                text: qsTr("Wait for the SMS containing the activation code and press 'Sign Up'")
                font.pixelSize: Theme.fontSizeSmall
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            TextField
            {
                id: tffirstname
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
                placeholderText: qsTr("First Name")
            }

            TextField
            {
                id: tfflastname
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
                placeholderText: qsTr("Last Name (Optional)")
            }
        }
    }
}
