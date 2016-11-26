import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../model"

Column
{
    property Context context

    id: signup
    spacing: Theme.paddingMedium

    Item { width: parent.width; height: Theme.paddingLarge }

    Connections
    {
        target: context.telegram

        onPhoneCodeError: {
            txterrormsg.text = qsTr("ERROR: %1").arg(errormessage);

            btnsignup.enabled = true;
            btnsignup.text = qsTr("Sign Up");

            timdisablebutton.restart();
        }
    }

    Timer
    {
        id: timdisablebutton
        running: false
        interval: 2000
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
        id: tfirstname
        anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
        placeholderText: qsTr("First Name")
    }

    TextField
    {
        id: tflastname
        anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
        placeholderText: qsTr("Last Name (Optional)")
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
        id: btnsignup
        anchors { horizontalCenter: parent.horizontalCenter }
        text: qsTr("Sign Up")
        enabled: (tfirstname.text.length > 0) && (tfcode.text.length > 0)

        onClicked: {
            btnsignup.enabled = false;
            btnsignup.text = qsTr("Sending request...");
            context.telegram.signUp(tfirstname.text, tflastname.text, tfcode.text);
        }
    }
}
