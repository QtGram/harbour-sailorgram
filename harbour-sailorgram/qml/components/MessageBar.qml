import QtQuick 2.1
import Sailfish.Silica 1.0

Item
{
    id: messagebar
    height: textarea.height

    Row
    {
        anchors.fill: parent

        TextArea
        {
            id: textarea
            width: parent.width - (btnallegate.visible ? btnallegate.width : btnsend.width)
            placeholderText: qsTr("Message") + "..."
            focusOutBehavior: FocusBehavior.KeepFocus
            labelVisible: false
        }

        IconButton
        {
            id: btnsend
            anchors.bottom: parent.bottom
            visible: textarea.text.length > 0;
            icon.source: "image://theme/icon-m-message"

            onClicked: {
                Qt.inputMethod.commit();
                messagemodel.sendMessage(textarea.text, (messagemodel.count > 0 ? true : false));
                textarea.text = "";

            }
        }

        IconButton
        {
            id: btnallegate
            anchors.bottom: parent.bottom
            visible: textarea.text.length <= 0;
            icon.source: "image://theme/icon-m-attach"
        }
    }
}
