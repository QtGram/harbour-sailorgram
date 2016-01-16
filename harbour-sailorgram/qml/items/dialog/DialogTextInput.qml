import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../components"
import "../../js/TelegramHelper.js" as TelegramHelper

InverseMouseArea
{
    property MessagesModel messagesModel
    property Context context
    property var dialog

    function sendMessage() {
        messagesModel.sendMessage(textarea.text.trim());
        Qt.inputMethod.commit();
        textarea.text = "";
    }

    id: dialogtextinput

    height: {
        if(!context.sailorgram.connected)
            return Theme.itemSizeSmall;

        return lbltimestamp.y + lbltimestamp.height + Theme.paddingSmall;
    }

    onClickedOutside: {
        textarea.focus = false;
    }

    ConnectingLabel
    {
        anchors { top: parent.top; bottom: parent.bottom; horizontalCenter: parent.horizontalCenter }
        context: dialogtextinput.context
    }

    TextArea
    {
        id: textarea
        visible: context.sailorgram.connected
        focusOutBehavior: FocusBehavior.KeepFocus
        font.pixelSize: Theme.fontSizeSmall
        placeholderText: qsTr("Message...")
        textRightMargin: 0

        anchors {
            left: parent.left
            right: row.left
            top: parent.top
            topMargin: Theme.paddingMedium
        }

        EnterKey.onClicked: {
            if(!context.sendwithreturn)
                return;

            sendMessage();
        }
    }

    Row
    {
        id: row
        visible: context.sailorgram.connected

        anchors {
            right: parent.right
            verticalCenter: textarea.top
            verticalCenterOffset: textarea.textVerticalCenterOffset + (textarea._editor.height - (Math.max(btnsend.height, btnselectmedia.height) / 4))
        }

        IconButton
        {
            id: btnsend
            visible: !context.sendwithreturn && textarea.text.length > 0
            icon.source: "image://theme/icon-m-message"
            onClicked: sendMessage()
        }

        IconButton
        {
            id: btnselectmedia
            visible: textarea.text.length <= 0
            icon.source: "image://theme/icon-m-attach"

            onClicked: {
                var returnpage = pageStack.currentPage;
                var selector = pageStack.push(Qt.resolvedUrl("../../pages/selector/SelectorMainPage.qml"), { "context": dialogtextinput.context });

                selector.actionCompleted.connect(function(action, data) {
                    var peerid = TelegramHelper.peerId(dialogtextinput.dialog);

                    if(action === selector.locationAction)
                        context.telegram.sendGeo(peerid, data.latitude, data.longitude);
                    else if(action === selector.soundRecordAction)
                        context.telegram.sendFile(peerid, data, false, true);
                    else
                        context.telegram.sendFile(peerid, data); // NOTE: Needs Investigation

                    pageStack.pop(returnpage);
                });
            }
        }
    }

    Label
    {
        id: lbltimestamp
        visible: context.sailorgram.connected
        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeTiny
        width: parent.width

        anchors {
            top: textarea.bottom
            topMargin: -textarea._labelItem.height - 3
            left: textarea.left
            leftMargin: Theme.horizontalPageMargin
            right: textarea.right
        }

        function updateTimestamp() {
            var date = new Date();
            text = Format.formatDate(date, Formatter.TimeValue);
            updater.interval = (60 - date.getSeconds() + 1) * 1000;
        }

        Timer {
            id: updater
            repeat: true
            triggeredOnStart: true
            running: Qt.application.active && lbltimestamp.visible
            onTriggered: lbltimestamp.updateTimestamp()
        }
    }
}
