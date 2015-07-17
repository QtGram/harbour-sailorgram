import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../components"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property Context context
    property Dialog dialog

    id: messagebar
    height: context.heartbeat.connected ? Math.max(textarea.height, btnselectmedia.height) : Theme.itemSizeSmall

    function sendMessage()
    {
        Qt.inputMethod.commit();
        messagemodel.sendMessage(textarea.text, (messagemodel.count > 0 ? true : false));
        textarea.text = "";
    }

    ConnectingLabel
    {
        anchors { top: parent.top; bottom: parent.bottom; horizontalCenter: parent.horizontalCenter }
        context: messagebar.context
    }

    Row
    {
        anchors.fill: parent
        visible: context.heartbeat.connected

        TextArea
        {
            id: textarea
            width: parent.width - (btnselectmedia.visible ? btnselectmedia.width : btnsend.width)
            anchors.verticalCenter: parent.verticalCenter
            placeholderText: qsTr("Message...")
            focusOutBehavior: FocusBehavior.KeepFocus
            font.pixelSize: Theme.fontSizeSmall
            labelVisible: false

            Keys.onReturnPressed: {
                if(!context.sendwithreturn)
                    return;

                sendMessage();
            }
        }

        IconButton
        {
            id: btnsend
            anchors.bottom: parent.bottom
            visible: !context.sendwithreturn && textarea.text.length > 0;
            width: visible ? Theme.itemSizeSmall : 0
            icon.source: "image://theme/icon-m-message"
            onClicked: sendMessage()
        }

        IconButton
        {
            id: btnselectmedia
            anchors.bottom: parent.bottom
            visible: textarea.text.length <= 0
            width: visible ? Theme.itemSizeSmall : 0
            icon.source: "image://theme/icon-m-attach"

            onClicked: {
                var picker = pageStack.push(Qt.resolvedUrl("../../pages/picker/FilePickerPage.qml"), { "rootPage": secretconversationpage });

                picker.filePicked.connect(function(file) {
                    context.telegram.sendFile(TelegramHelper.peerId(dialog), file);
                });
            }
        }
    }
}
