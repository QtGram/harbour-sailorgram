import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../models"
import "../../components"
import "../../js/TelegramHelper.js" as TelegramHelper

InverseMouseArea
{
    property MessageListModel messageListModel
    property Context context
    property var peer
    property Message replyMessage
    property bool isForward

    property Status inputStatus: Status {
        engine: context.engine

        typing {
            peer: dialogtextinput.peer

            action {
                classType: SendMessageAction.TypeSendMessageCancelAction
            }
        }
    }

    signal messageSent()
    signal forwardRequested()

    function focusTextArea() {
        textarea.forceActiveFocus();
    }

    function sendMessage() {
        if(!isForward) {
            var sendtext = context.sendwithreturn ? (textarea.text.trim().replace(/\r\n|\n|\r/gm, "")) : textarea.text.trim();
            messageListModel.sendMessage(sendtext, replyMessage);
        }
        else
            forwardRequested();

        Qt.inputMethod.commit();
        textarea.text = "";
        messageSent();
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

    Timer
    {
        id: typingtimer
        interval: 3000

        onTriggered: {
            if(inputStatus.typing.action.classType === SendMessageAction.TypeSendMessageTypingAction)
                inputStatus.typing.action.classType = SendMessageAction.TypeSendMessageCancelAction;
        }

        function startTyping() {
            if(!running) {
                start();
                inputStatus.typing.action.classType = SendMessageAction.TypeSendMessageTypingAction;
            }
        }
    }

    TextArea
    {
        id: textarea
        visible: context.sailorgram.connected
        focusOutBehavior: FocusBehavior.KeepFocus
        font.pixelSize: Theme.fontSizeSmall
        placeholderText: qsTr("Message...")
        textRightMargin: 0
        onTextChanged: typingtimer.startTyping()

        anchors {
            left: parent.left
            right: row.left
            top: parent.top
            topMargin: Theme.paddingMedium
        }

        EnterKey.enabled: text.trim().length > 0 || isForward

        EnterKey.iconSource: context.sendwithreturn ? "image://theme/icon-m-enter-accept" : "image://theme/icon-m-enter"

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
            visible: isForward || (!context.sendwithreturn && textarea.text.length > 0)
            icon.source: "image://theme/icon-m-message"
            onClicked: sendMessage()
        }

        IconButton
        {
            id: btnselectmedia
            visible: !isForward && textarea.text.length <= 0
            icon.source: "image://theme/icon-m-attach"

            onClicked: {
                var returnpage = pageStack.currentPage;
                var selector = pageStack.push(Qt.resolvedUrl("../../pages/selector/SelectorMainPage.qml"), { "context": dialogtextinput.context,
                                                                                                             "dialogPage": pageStack.currentPage });

                selector.actionRequested.connect(function(actiontype) {
                    inputStatus.typing.action.classType = actiontype;
                });

                selector.actionRejected.connect(function() {
                    inputStatus.typing.action.classType = SendMessageAction.TypeSendMessageCancelAction;
                });

                selector.actionCompleted.connect(function(actiontype, data) {
                    var peerid = 0;

                    if(actiontype === SendMessageAction.TypeSendMessageGeoLocationAction)
                        context.telegram.sendGeo(peerid, data.latitude, data.longitude); //FIXME: sendGeo() API Missing
                    else if(actiontype === SendMessageAction.TypeSendMessageRecordAudioAction)
                        messageListModel.sendFile(Enums.SendFileTypeAudio, data, replyMessage);
                    else if(actiontype === SendMessageAction.TypeSendMessageUploadDocumentAction)
                        messageListModel.sendFile(Enums.SendFileTypeDocument, data, replyMessage);
                    else if(actiontype === selector.stickerAction) // Special
                        messageListModel.sendSticker(data, replyMessage);
                    else
                        console.log("Unhandled actiontype: " + actiontype);

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
