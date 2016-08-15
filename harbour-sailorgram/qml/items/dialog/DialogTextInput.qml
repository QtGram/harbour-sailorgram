import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0
import "../../models"
import "../../components"
import "../../js/TelegramHelper.js" as TelegramHelper

InverseMouseArea
{
    property Context context
    property SailorgramMessageModel sgMessageModel
    property SailorgramMessageItem sgReplyMessage
    property bool isForward: false

    signal messageSent()
    signal forwardRequested()

    function focusTextArea() {
        textarea.forceActiveFocus();
    }

    function sendMessage() {
        if(!isForward) {
            var sendtext = context.sendwithreturn ? (textarea.text.trim().replace(/\r\n|\n|\r/gm, "")) : textarea.text.trim();
            sgMessageModel.sendMessage(sendtext, sgReplyMessage);
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

    TextArea
    {
        id: textarea
        visible: context.sailorgram.connected
        focusOutBehavior: FocusBehavior.KeepFocus
        font.pixelSize: Theme.fontSizeSmall
        placeholderText: qsTr("Message...")
        textRightMargin: 0

        onTextChanged: {
            sgMessageModel.typingStatus = SailorgramEnums.TypingStatusTyping;
        }

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

                selector.actionRequested.connect(function(typingstatus) { sgMessageModel.typingStatus = typingstatus; });
                selector.actionRejected.connect(function() { sgMessageModel.typingStatus = SailorgramEnums.TypingStatusCancel; });

                selector.actionCompleted.connect(function(typingstatus, data) {
                    if(typingstatus === SailorgramEnums.TypingStatusGeoLocation)
                        console.log("Placehoder");
                        //context.telegram.sendGeo(peerid, data.latitude, data.longitude); //FIXME: sendGeo() API Missing
                    else if(typingstatus === SailorgramEnums.TypingStatusRecordAudio)
                        sgMessageModel.sendFile(SailorgramEnums.SendFileTypeAudio, data, sgReplyMessage);
                    else if(typingstatus === SailorgramEnums.TypingStatusUploadDocument)
                        sgMessageModel.sendFile(SailorgramEnums.SendFileTypeDocument, data, sgReplyMessage);
                    else if(typingstatus === selector.stickerAction) // Special
                        sgMessageModel.sendSticker(data, sgReplyMessage);
                    else
                        console.log("Unhandled typingstatus: " + typingstatus);

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
