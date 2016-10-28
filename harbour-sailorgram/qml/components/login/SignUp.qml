import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../model"

Column
{
    property Context context
    property bool authError: false

    id: signup
    spacing: Theme.paddingMedium

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
        text: authError ? qsTr("You have entered a wrong Authorization Code") : qsTr("Wait for the SMS containing the activation code and press 'Sign Up'")
        font.pixelSize: Theme.fontSizeSmall
        anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
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

        onTextChanged: {
            if(authError)
                authError = false; /* Reset Error State */
        }
    }

    Button
    {
        id: btnsignup
        anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
        text: qsTr("Sign Up")
        enabled: (tfirstname.text.length > 0) && (tfcode.text.length > 0)

        onClicked: {
            btnsignup.enabled = false;
            btnsignup.text = qsTr("Sending request...");
            context.telegram.signUp(tfirstname.text, tflastname.text, tfcode.text);
        }
    }
}
