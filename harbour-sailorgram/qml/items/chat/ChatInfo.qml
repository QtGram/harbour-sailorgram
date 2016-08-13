import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../user"
import "../../components"
import "../../models"
import "../../menus/chat"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property Context context
    property Chat chat
    property var peer

    id: chatinfo
    width: content.width
    height: column.height + lvpartecipants.contentHeight

    Column
    {
        id: column
        width: parent.width

        ClickableLabel
        {
            width: parent.width
            height: Theme.itemSizeSmall
            labelText: qsTr("Leave group")
            labelFont.pixelSize: Theme.fontSizeSmall
            remorseRequired: true
            remorseMessage: qsTr("Leaving group")

            onActionRequested: {
                /* FIXME:
                var peerid = TelegramHelper.peerId(dialog);
                context.telegram.messagesDeleteChatUser(peerid, context.telegram.me);
                context.telegram.messagesDeleteHistory(peerid);
                pageStack.pop();
                */
            }
        }

        ClickableLabel
        {
            visible: chat.admin
            width: parent.width
            height: Theme.itemSizeSmall
            labelText: qsTr("Change title")
            labelFont.pixelSize: Theme.fontSizeSmall
            onActionRequested: pageStack.push(Qt.resolvedUrl("../../pages/chat/ChangeChatTitle.qml"), { "context": chatinfo.context, "dialog": chatinfo.dialog })
        }

        ClickableLabel
        {
            visible: chat.admin
            width: parent.width
            height: Theme.itemSizeSmall
            labelText: qsTr("Add member")
            labelFont.pixelSize: Theme.fontSizeSmall
            onActionRequested: pageStack.push(Qt.resolvedUrl("../../pages/chat/AddContactsPage.qml"), { "context": chatinfo.context, "dialog": chatinfo.dialog })
        }
    }

    SilicaListView
    {
        id: lvpartecipants
        spacing: Theme.paddingMedium
        anchors { left: parent.left; top: column.bottom; right: parent.right; bottom: parent.bottom }

        model: MembersListModel {
            id: chatparticipantsmodel
            engine: context.engine
            currentPeer: chatinfo.peer
        }

        header: SectionHeader { id: secheader; text: qsTr("Members") }

        delegate: ListItem {
            id: liparticipant
            contentWidth: parent.width
            contentHeight: Theme.itemSizeSmall
            onClicked: pageStack.push(Qt.resolvedUrl("../../pages/contacts/ContactPage.qml"), { "context": chatinfo.context, "user": model.user } )

            menu: ChatInfoMenu {
                context: chatinfo.context
                admin: chatinfo.chat.admin
                user: model.user
                peer: model.peer
            }

            UserItem {
                id: useritem
                anchors { fill: parent; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
                context: chatinfo.context
                statusText: model.status
                user: model.user
            }
        }
    }
}
