import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../models"
import "../js/TelegramHelper.js" as TelegramHelper

Item
{
    property Settings settings
    property Telegram telegram
    property Dialog dialog

    id: messagebar
    height: textarea.height

    Row
    {
        anchors.fill: parent

        TextArea
        {
            id: textarea
            width: parent.width - (btnselectmedia.visible ? btnselectmedia.width : btnsend.width)
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
            id: btnselectmedia
            anchors.bottom: parent.bottom
            visible: textarea.text.length <= 0;
            icon.source: "image://theme/icon-m-attach"

            onClicked: {
                var picker = pageStack.push(Qt.resolvedUrl("../pages/picker/FilePickerPage.qml"), { "rootPage": conversationpage });

                picker.filePicked.connect(function(file) {
                    telegram.sendFile(TelegramHelper.peerId(dialog.peer), file);
                });
            }
        }
    }
}
