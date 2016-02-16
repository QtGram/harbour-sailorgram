import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../message/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/ColorScheme.js" as ColorScheme

Item
{
    property Context context
    property Dialog dialog
    property Message message

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
        anchors { top: parent.top; right: parent.right; bottom: parent.bottom; rightMargin: Theme.paddingSmall }
        icon.source: "qrc:///res/close.png"
        icon.sourceSize: Qt.size(width * 0.5, height * 0.5)

        onClicked: {
            dialogreplypreview.message = null;
            closeRequested();
        }
    }
}
