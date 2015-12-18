import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../menus/dialog"
import "../../components"
import "../../items/peer"
import "../../items/dialog"
import "../../items/message"
import "../../items/message/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property Context context
    property Dialog dialog
    property Chat chat
    property User user

    id: dialogpage
    allowedOrientations: defaultAllowedOrientations

    Component.onCompleted: {
        if(TelegramHelper.isChat(dialog))
            chat = context.telegram.chat(dialog.peer.chatId);
        else
            user = context.telegram.user(dialog.peer.userId);
    }

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        pageStack.pushAttached(Qt.resolvedUrl("DialogInfoPage.qml"), { "context": dialogpage.context, "dialog": dialogpage.dialog, "chat": dialogpage.chat, "user": dialogpage.user });
        context.foregroundDialog = dialogpage.dialog;

        messagemodel.telegram = dialogpage.context.telegram;
        messagemodel.dialog = dialogpage.dialog;
        messagemodel.setReaded();
    }

    RemorsePopup { id: remorsepopup }

    Timer
    {
        id: refreshtimer
        repeat: true
        interval: 10000
        onTriggered: messagemodel.refresh()
        Component.onCompleted: start()
    }

    PopupMessage
    {
        id: popupmessage
        anchors { left: parent.left; top: parent.top; right: parent.right }
    }

    BusyIndicator
    {
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
        running: messagemodel.refreshing
        z: running ? 2 : 0
    }

    PeerItem
    {
        id: header
        visible: !context.chatheaderhidden
        anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.horizontalPageMargin; topMargin: context.chatheaderhidden ? 0 : Theme.paddingMedium }
        height: context.chatheaderhidden ? 0 : (dialogpage.isPortrait ? Theme.itemSizeSmall : Theme.itemSizeExtraSmall)
        context: dialogpage.context
        dialog: dialogpage.dialog
        chat: dialogpage.chat
        user: dialogpage.user
    }

    MessageView
    {
        id: messageview
        anchors { left: parent.left; top: header.bottom; right: parent.right; bottom: parent.bottom }
        context: dialogpage.context

        model: MessagesModel {
            id: messagemodel

            onCountChanged: {
                if(!count)
                    return;

                messagemodel.setReaded(); /* We are in this chat, always mark these messages as read */
            }
        }

        delegate: MessageItem {
            context: dialogpage.context
            message: item
        }

        header: Item {
            width: messageview.width
            height: dialogtextinput.height
        }

        Column {
            id: headerarea
            y: messageview.headerItem.y
            parent: messageview.contentItem
            width: parent.width

            DialogTextInput {
                id: dialogtextinput
                width: parent.width
                context: dialogpage.context
                dialog: dialogpage.dialog
            }
        }
    }
}
