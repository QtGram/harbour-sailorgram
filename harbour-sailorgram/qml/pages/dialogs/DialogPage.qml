import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0
import harbour.sailorgram.TelegramQml 2.0 as Telegram
import "../../models"
import "../../components"
import "../../items/peer"
import "../../items/dialog"
import "../../items/message"
import "../../items/message/messageitem"
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property Context context
    property SailorgramDialogItem sgDialogItem
    property alias sgForwardMessage: messageview.sgForwardMessage

    id: dialogpage
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        if(!canNavigateForward)
            pageStack.pushAttached(Qt.resolvedUrl("DialogInfoPage.qml"), { "context": dialogpage.context,
                                                                           "sgDialogItem": dialogpage.sgDialogItem });

        context.notifications.currentDialog = dialogpage.sgDialogItem;
    }

    PopupMessage
    {
        id: popupmessage
        anchors { left: parent.left; top: parent.top; right: parent.right }
    }

    RemorsePopup { id: remorsepopup }

    SilicaFlickable
    {
        anchors.fill: parent

        PullDownMenu
        {
            id: pulldownmenu

            MenuItem
            {
                text: messageview.selectionMode ? qsTr("Cancel selection") : qsTr("Select messages")
                onClicked: messageview.selectionMode = !messageview.selectionMode
            }

            MenuItem
            {
                text: qsTr("Select all")
                visible: messageview.selectionMode && messageview.selectedItems.length !== messageview.model.count
                onClicked: messageview.selectAll()
            }

            MenuItem
            {
                text: qsTr("Clear selection")
                visible: messageview.selectedItems.length > 0
                onClicked: messageview.clearSelection()
            }

            MenuItem
            {
                text: qsTr("Load more messages")
                onClicked: messageview.loadBack();
            }
        }

        PeerItem
        {
            id: header
            visible: !context.chatheaderhidden
            height: context.chatheaderhidden ? 0 : (dialogpage.isPortrait ? Theme.itemSizeSmall : Theme.itemSizeExtraSmall)
            context: dialogpage.context
            isSecretChat: sgDialogItem.isSecretChat
            peer: sgDialogItem.peer
            title: sgDialogItem.title
            isChat: sgDialogItem.isChat
            typingUsers: sgDialogItem.typingUsers
            statusText: sgDialogItem.statusText

            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
                leftMargin: Theme.horizontalPageMargin
                topMargin: context.chatheaderhidden ? 0 : Theme.paddingMedium
            }
        }

        MessageView
        {
            id: messageview
            anchors { left: parent.left; top: header.bottom; right: parent.right; bottom: selectionactions.top }
            context: dialogpage.context
            sgDialogItem: dialogpage.sgDialogItem

            discadedDialog: {
                if(!dialogpage.sgDialogItem.isSecretChat)
                    return false;

                return dialogpage.sgDialogItem.secretChatState === SailorgramEnums.SecretChatStateCancelled;
            }

            waitingDialog: {
                if(!dialogpage.sgDialogItem.isSecretChat)
                    return false;

                return dialogpage.sgDialogItem.secretChatState === SailorgramEnums.SecretChatStateRequested;
            }

            waitingUserName: {
                if(!dialogpage.sgDialogItem.isSecretChat)
                    return false;

                if(dialogpage.sgDialogItem.secretChatState !== SailorgramEnums.SecretChatStateRequested)
                    return "";

                return TelegramHelper.completeName(dialogpage.sgDialogItem.title);
            }

            onSelectedItemsChanged: {
                selectionactions.open = selectedItems.length > 0;
            }
        }

        DockedPanel
        {
            id: selectionactions
            width: parent.width
            height: deleteselected.height + Theme.paddingMedium
            open: false
            visible: visibleSize > 0.0

            onOpenChanged: {
                if(!open && messageview.selectedItems.length > 0)
                    messageview.selectionMode = false;
            }

            Image
            {
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source: "image://theme/graphic-gradient-edge"
            }

            IconButton
            {
                id: deleteselected
                anchors.centerIn: parent
                icon.source: "image://theme/icon-m-delete?" + (pressed ? Theme.highlightColor : Theme.primaryColor)

                onClicked: remorsepopup.execute(qsTr("Deleting Messages"), function() {
                    messageview.deleteSelected();
                    messageview.selectionMode = false;
                })
            }
        }
    }
}
