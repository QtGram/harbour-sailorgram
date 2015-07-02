import QtQuick 2.1
import Sailfish.Silica 1.0

Dialog
{
    property alias authError: tfcode.errorHighlight
    property var telegram

    id: dlgsignup
    allowedOrientations: defaultAllowedOrientations
    canAccept: (tffirstname.text.length > 0) && (tfcode.text.length > 0)

    acceptDestination: Component {
        ConnectionPage {
            telegram: dlgsignup.telegram
        }
    }

    onAccepted: {
        telegram
    }

    SilicaFlickable
    {
        id: flickable
        anchors.fill: parent
        contentHeight: content.height

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
                text: authError ? qsTr("You have entered a wrong Authorization Code") : qsTr("Wait for the SMS containing the activation code and press 'Sign Up'")
                font.pixelSize: Theme.fontSizeSmall
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            TextField
            {
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
                placeholderText: qsTr("First Name")
            }

            TextField
            {
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
                placeholderText: qsTr("Last Name (Optional)")
            }

            TextField
            {
                id: tfcode
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
                placeholderText: qsTr("Authorization Code")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly

                onTextChanged: {
                    if(authError)
                        authError = false; /* Reset Error State */
                }
            }
        }
    }
}
