import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../user"
import "../../components"
import "../../models"
import "../../menus/chat"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property Context context
    property Dialog dialog
    property Chat chat
    property ChatFull chatFull: context.telegram.chatFull(chat.id)
    property bool adminMenu: false

    onChatFullChanged: {
        adminMenu = (chatFull.participants.adminId === context.telegram.me);
    }

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
                var peerid = TelegramHelper.peerId(dialog);
                context.telegram.messagesDeleteChatUser(peerid, context.telegram.me);
                context.telegram.messagesDeleteHistory(peerid);
                pageStack.pop();
            }
        }

        ClickableLabel
        {
            width: parent.width
            height: Theme.itemSizeSmall
            labelText: qsTr("Change title")
            labelFont.pixelSize: Theme.fontSizeSmall
            onActionRequested: pageStack.push(Qt.resolvedUrl("../../pages/chat/ChangeChatTitle.qml"), { "context": chatinfo.context, "dialog": chatinfo.dialog })
        }

        ClickableLabel
        {
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

        model: ChatParticipantsModel {
            id: chatparticipantsmodel
            telegram: context.telegram
            dialog: chatinfo.dialog
        }

        header: SectionHeader { id: secheader; text: qsTr("Members") }

        delegate: ListItem {
            property ChatParticipant participant: item
            property User user: context.telegram.user(participant.userId)

            id: liparticipant
            contentWidth: parent.width
            contentHeight: Theme.itemSizeSmall
            showMenuOnPressAndHold: adminMenu && (user.id !== context.telegram.me)

            onClicked: pageStack.push(Qt.resolvedUrl("../../pages/contacts/ContactPage.qml"), { "context": chatinfo.context, "user": liparticipant.user } )

            menu: ChatInfoMenu {
                context: chatinfo.context
                dialog: chatinfo.dialog
                user: liparticipant.user
            }

            UserItem {
                id: useritem
                anchors { fill: parent; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
                context: chatinfo.context
                user: liparticipant.user
            }
        }
    }
}
