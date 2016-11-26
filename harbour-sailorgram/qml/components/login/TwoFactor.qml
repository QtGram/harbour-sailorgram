import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../model"

Column
{
    property Context context
    property string hint

    id: twofactor
    spacing: Theme.paddingMedium

    Item { width: parent.width; height: Theme.paddingLarge }

    Connections
    {
        target: context.telegram

        onInvalidPassword: {
            txtinvalidpassword.visible = true;
            btnsubmit.text = qsTr("Submit");
            btnsubmit.enabled = true;
        }
    }

    Image
    {
        id: imgwaiting
        source: "qrc:///res/twofactor.png"
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Label
    {
        id: lblinfo
        text: qsTr("Please enter your cloud password")
        font.pixelSize: Theme.fontSizeSmall
        anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
    }

    Label
    {
        id: txtinvalidpassword
        font { bold: true; underline: true }
        color: Theme.secondaryHighlightColor
        text: qsTr("Invalid password")
        visible: false
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
    }

    TextField
    {
        id: tfcloudpassword
        anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
        placeholderText: qsTr("Your cloud password")
        echoMode: TextInput.Password
    }

    Label
    {
        text: qsTr("Hint: %1").arg(hint)
        visible: hint.length > 0
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
    }

    Button
    {
        id: btnsubmit
        anchors { horizontalCenter: parent.horizontalCenter }
        text: qsTr("Submit")
        enabled: tfcloudpassword.text.length > 0

        onClicked: {
            txtinvalidpassword.visible = false;
            btnsubmit.enabled = false;
            btnsubmit.text = qsTr("Sending request...");
            context.telegram.sendPassword(tfcloudpassword.text);
        }
    }
}
