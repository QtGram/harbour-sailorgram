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
    property Message message

    signal closeRequested()

    id: dialogreplypreview
    height: messagepreview.height
    visible: message !== null

    MessagePreview
    {
        id: messagepreview
        context: dialogreplypreview.context
        message: dialogreplypreview.message
        showQuoteRect: false

        anchors {
            left: parent.left
            top: parent.top
            right: btnclose.left
            leftMargin: Theme.paddingSmall
            rightMargin: Theme.paddingSmall
            topMargin: Theme.paddingSmall
        }
    }

    IconButton
    {
        id: btnclose
        width: parent.height
        anchors { top: parent.top; right: parent.right; bottom: parent.bottom; rightMargin: Theme.paddingSmall }
        icon.source: "qrc:///res/close.png"
        icon.sourceSize: Qt.size(width * 0.5, height * 0.5)

        onClicked: {
            dialogreplypreview.message = null;
            closeRequested();
        }
    }
}

