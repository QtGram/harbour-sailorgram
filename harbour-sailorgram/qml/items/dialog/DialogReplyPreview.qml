import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../models"
import "../message/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/ColorScheme.js" as ColorScheme

Item
{
    property alias context: messagepreview.context
    property alias sgMessageItem: messagepreview.sgMessageItem
    property bool isForward: false

    signal closeRequested()

    id: dialogreplypreview
    height: messagepreview.height
    visible: sgMessageItem !== null

    MessagePreview
    {
        id: messagepreview
        titlePrefix: isForward ? qsTr("Message to forward") : qsTr("Reply to")
        width: parent.width - btnclose.width - Theme.paddingSmall
        sgPeer: messagepreview.sgMessageItem ? messagepreview.sgMessageItem.fromPeer : null
        showQuoteRect: false
        showUser: !isForward

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
            dialogreplypreview.sgMessageItem = null;
            closeRequested();
        }
    }
}
