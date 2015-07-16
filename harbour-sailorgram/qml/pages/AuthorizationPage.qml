import QtQuick 2.1
import Sailfish.Silica 1.0
import "../models"
import "../js/Settings.js" as Settings

Dialog
{
    property alias authError: tfcode.errorHighlight
    property Context context

    id: dlgauthorization
    allowedOrientations: defaultAllowedOrientations
    canAccept: tfcode.text.length > 0
    acceptDestinationAction: PageStackAction.Replace
    onAccepted: context.telegram.authSignIn(tfcode.text)

    acceptDestination: Component {
        ConnectionPage {
            context: dlgauthorization.context
        }
    }

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: dlgheader.height + infocontainer.height + tfcode.height

        RemorsePopup { id: remorsepopup }

        PushUpMenu
        {
            MenuItem
            {
                text: qsTr("Resend Activation Code")

                onClicked: {
                    remorsepopup.execute(qsTr("Sending Activation Code"), function() {
                        context.telegram.authSendCode();
                    });
                }
            }
        }

        DialogHeader
        {
            id: dlgheader
            acceptText: qsTr("Sign In")
        }

        Item
        {
            id: infocontainer
            anchors { left: parent.left; top: dlgheader.bottom; right: parent.right }
            height: lblinfo.contentHeight + imgwaiting.height + Theme.paddingLarge

            Image
            {
                id: imgwaiting
                source: "qrc:///res/waiting.png"
                anchors { top: parent.top; horizontalCenter: parent.horizontalCenter }
            }

            Label
            {
                id: lblinfo
                text: authError ? qsTr("You have entered a wrong Authorization Code") : qsTr("Wait for the SMS containing the activation code and press 'Sign In'")
                font.pixelSize: Theme.fontSizeSmall
                anchors { left: parent.left; top: imgwaiting.bottom; right: parent.right; topMargin: Theme.paddingLarge; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
        }

        TextField
        {
            id: tfcode
            anchors { left: parent.left; top: infocontainer.bottom; right: parent.right; topMargin: Theme.paddingLarge }
            placeholderText: qsTr("Authorization Code")
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly

            onTextChanged: {
                if(authError)
                    authError = false; /* Reset Error State */
            }
        }
    }
}
