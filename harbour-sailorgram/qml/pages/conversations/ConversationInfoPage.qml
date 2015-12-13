import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../components"
import "../../items/peer"
import "../../items/user"
import "../../items/chat"
import "../../js/TelegramConstants.js" as TelegramConstants
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property bool muted: context.telegram.userData.isMuted(TelegramHelper.peerId(conversationinfopage.dialog))
    property bool actionVisible: true
    property Context context
    property Dialog dialog
    property Chat chat
    property User user

    Connections
    {
        target: context.telegram.userData

        onMuteChanged: {
            if(id !== user.id)
                return;

            conversationinfopage.muted = context.telegram.userData.isMuted(id);
        }
    }

    id: conversationinfopage
    allowedOrientations: defaultAllowedOrientations

    function conversationTypeMessage() {
        if(dialog.encrypted)
            return qsTr("Delete secret chat");

        if(TelegramHelper.isChat(dialog))
            return qsTr("Delete group");

        return qsTr("Delete conversation");
    }

    function conversationTypeRemorseMessage() {
        if(dialog.encrypted)
            return qsTr("Deleting secret chat");

        if(TelegramHelper.isChat(dialog))
            return qsTr("Deleting group");

        return qsTr("Deleting conversation");
    }

    Component {
        id: userinfocomponent

        UserInfo {
            actionVisible: true
            allowSendMessage: false
            context: conversationinfopage.context
            user: conversationinfopage.user
        }
    }

    Component {
        id: chatinfocomponent

        ChatInfo {
            context: conversationinfopage.context
            dialog: conversationinfopage.dialog
            chat: conversationinfopage.chat
        }
    }

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        Column
        {
            id: content
            width: parent.width

            PageHeader { title: TelegramHelper.isChat(dialog) ? chat.title : TelegramHelper.completeName(user) }

            PeerItem
            {
                x: Theme.paddingMedium
                width: parent.width - (x * 2)
                height: Theme.itemSizeSmall
                context: conversationinfopage.context
                dialog: conversationinfopage.dialog
                chat: conversationinfopage.chat
                user: conversationinfopage.user
            }

            SectionHeader { text: qsTr("Actions") }

            ClickableLabel
            {
                labelText: conversationinfopage.muted ? qsTr("Enable notifications") : qsTr("Disable notifications")
                labelFont.pixelSize: Theme.fontSizeSmall
                width: parent.width
                height: Theme.itemSizeSmall

                onActionRequested: {
                    if(conversationinfopage.dialog.encrypted) { // Secret chats are P2P
                        if(context.telegram.userData.isMuted(conversationinfopage.dialog.peer.userId)) {
                            context.telegram.userData.removeMute(conversationinfopage.dialog.peer.userId);
                            conversationinfopage.muted = false;
                        }
                        else {
                            context.telegram.userData.addMute(conversationinfopage.dialog.peer.userId);
                            conversationinfopage.muted = true;
                        }

                        return;
                    }

                    var peerid = TelegramHelper.peerId(conversationinfopage.dialog);

                    if(context.telegram.userData.isMuted(peerid)) {
                        context.telegram.unmute(peerid);
                        conversationinfopage.muted = false;
                    }
                    else {
                        context.telegram.mute(peerid);
                        conversationinfopage.muted = true;
                    }
                }
            }

            ClickableLabel
            {
                labelText: conversationTypeMessage()
                labelFont.pixelSize: Theme.fontSizeSmall
                remorseMessage: conversationTypeRemorseMessage()
                remorseRequired: true
                width: parent.width
                height: Theme.itemSizeSmall

                onActionRequested: {
                    dialog.unreadCount = 0;

                    if(dialog.encrypted)
                        context.telegram.messagesDiscardEncryptedChat(dialog.peer.userId);
                    else
                        context.telegram.messagesDeleteHistory(TelegramHelper.peerId(dialog));

                    pageStack.pop();
                }
            }

            ClickableLabel
            {
                labelText: qsTr("Add to contacts")
                labelFont.pixelSize: Theme.fontSizeSmall
                visible: !TelegramHelper.isChat(dialog) && !TelegramHelper.isTelegramUser(user) && (user.classType === TelegramConstants.typeUserRequest)
                width: parent.width
                height: Theme.itemSizeSmall
                onActionRequested: context.telegram.addContact(user.firstName, user.lastName, user.phone)
            }

            Loader
            {
                id: loader

                sourceComponent: {
                    if(TelegramHelper.isChat(dialog))
                        return chatinfocomponent;

                    return userinfocomponent;
                }
            }
        }
    }
}
