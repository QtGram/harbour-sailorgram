import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../models"
import "../message/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/ColorScheme.js" as ColorScheme

Item
{
    property Context context
    property Dialog dialog
    property Message message
    property bool isForward

    signal closeRequested()

    id: dialogreplypreview
    height: messagepreview.height
    visible: message !== null

    MessagePreview
    {
        id: messagepreview
        context: dialogreplypreview.context
        dialog: dialogreplypreview.dialog
        message: dialogreplypreview.message
        showQuoteRect: false
        titlePrefix: isForward ? qsTr("Message to forward") : qsTr("Reply to")
        showUser: !isForward
        width: parent.width - btnclose.width - Theme.paddingSmall

        anchors {
            left: parent.left
            top: parent.top
            leftMargin: Theme.paddingSmall
            topMargin: Theme.paddingSmall
        }
    }

    IconButton
    {
        id: btnclose
        width: messagepreview.height
        anchors { top: parent.top; right: parent.right; bottom: parent.bottom }
        icon.source: "qrc:///res/close.png"
        icon.sourceSize: Qt.size(width * 0.5, height * 0.5)

        onClicked: {
            dialogreplypreview.message = null;
            closeRequested();
        }
    }
}
