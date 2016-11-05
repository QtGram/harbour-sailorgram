import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../custom"

InverseMouseArea
{
    property alias text: tamessage.text

    signal sendMessage(string message)

    function focusTextArea() {
        tamessage.forceActiveFocus();
    }

    function prepareMessage() {
        var msg = context.sendwithreturn ? (tamessage.text.trim().replace(/\r\n|\n|\r/gm, "")) : tamessage.text.trim();
        sendMessage(msg);

        Qt.inputMethod.commit();
        tamessage.text = "";
    }

    id: messagetextinput
    height: visible ? (lbltimestamp.y + lbltimestamp.height + Theme.paddingSmall) : 0

    onClickedOutside: {
        tamessage.focus = false;
    }

    BackgroundRectangle { anchors.fill: parent }

    TextArea
    {
        id: tamessage
        anchors { left: parent.left; top: parent.top; right: btnsend.left }
        focusOutBehavior: FocusBehavior.KeepFocus
        font.pixelSize: Theme.fontSizeSmall
        placeholderText: qsTr("Message...")
        textMargin: Theme.paddingMedium

        EnterKey.enabled: text.trim().length > 0
        EnterKey.iconSource: context.sendwithreturn ? "image://theme/icon-m-enter-accept" : "image://theme/icon-m-enter"

        EnterKey.onClicked: {
            if(!context.sendwithreturn)
                return;

            prepareMessage();
        }

    }

    IconButton
    {
        id: btnsend
        anchors.right: parent.right
        icon.source: (tamessage.text.length <= 0) ? "image://theme/icon-m-attach" : "image://theme/icon-m-message"
        onClicked: prepareMessage()
    }

    Label
    {
        id: lbltimestamp
        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeTiny

        anchors {
            top: tamessage.bottom
            topMargin: -tamessage._labelItem.height - 3
            left: tamessage.left
            leftMargin: tamessage.textLeftMargin
            right: tamessage.right
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
