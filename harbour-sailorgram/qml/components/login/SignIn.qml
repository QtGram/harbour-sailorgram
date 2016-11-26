import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../model"

Column
{
    property Context context

    id: signin
    spacing: Theme.paddingMedium

    Item { width: parent.width; height: Theme.paddingLarge }

    Connections
    {
        target: context.telegram

        onPhoneCodeError: {
            txterrormsg.text = qsTr("ERROR: %1").arg(errormessage);

            btnsignin.enabled = true;
            btnsignin.text = qsTr("Sign In");
        }
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
        text: qsTr("Wait for the SMS containing the activation code and press 'Sign In'")
        font.pixelSize: Theme.fontSizeSmall
        anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
    }

    Label
    {
        id: txterrormsg
        font { bold: true; underline: true; pixelSize: Theme.fontSizeSmall }
        color: Theme.secondaryHighlightColor
        visible: txterrormsg.visible
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
    }

    TextField
    {
        id: tfcode
        anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
        placeholderText: qsTr("Code")
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly
    }

    Button
    {
        id: btnsignin
        anchors { horizontalCenter: parent.horizontalCenter }
        text: qsTr("Sign In")
        enabled: tfcode.text.length > 0

        onClicked: {
            btnsignin.enabled = false;
            btnsignin.text = qsTr("Sending request...");
            context.telegram.signIn(tfcode.text);
        }
    }
}
