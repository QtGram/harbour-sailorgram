import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../models"
import "../../components"
import "../../items/peer"
import "../../items/user"
import "../../items/chat"
import "../../js/TelegramConstants.js" as TelegramConstants
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property bool actionVisible: true
    property Context context
    property var peer
    property Chat chat
    property User user
    property string statusText
    property string title
    property bool isSecretChat
    property bool mute

    id: dialoginfopage
    allowedOrientations: defaultAllowedOrientations

    function conversationTypeMessage() {
        if(isSecretChat)
            return qsTr("Delete secret chat");

        if(chat)
            return qsTr("Delete group");

        return qsTr("Delete conversation");
    }

    function conversationTypeRemorseMessage() {
        if(isSecretChat)
            return qsTr("Deleting secret chat");

        if(chat)
            return qsTr("Deleting group");

        return qsTr("Deleting conversation");
    }

    Component {
        id: userinfocomponent

        UserInfo {
            actionVisible: true
            allowSendMessage: false
            context: dialoginfopage.context
            user: dialoginfopage.user
        }
    }

    Component {
        id: chatinfocomponent

        ChatInfo {
            context: dialoginfopage.context
            chat: dialoginfopage.chat
            peer: dialoginfopage.peer
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

            PageHeader { title: qsTr("Chat information") }

            PeerProfile
            {
                id: peerprofile
                width: parent.width
                context: dialoginfopage.context
                peer: dialoginfopage.peer
                chat: dialoginfopage.chat
                isSecretChat: dialoginfopage.isSecretChat
                statusText: dialoginfopage.statusText
                fallbackAvatar: dialoginfopage.title

                title: {
                    if(dialoginfopage.chat)
                        return dialoginfopage.title;

                    var name = TelegramHelper.completeName(dialoginfopage.user);

                    if(dialoginfopage.user.username.length > 0)
                        name += " (@" + dialoginfopage.user.username + ")";

                    return name;
                }
            }

            SectionHeader { text: qsTr("Actions") }

            ClickableLabel
            {
                labelText: dialoginfopage.mute ? qsTr("Enable notifications") : qsTr("Disable notifications")
                labelFont.pixelSize: Theme.fontSizeSmall
                width: parent.width
                height: Theme.itemSizeSmall

                /* FIXME:
                onActionRequested: {
                    if(dialoginfopage.dialog.encrypted) { // Secret chats are P2P
                        if(context.telegram.userData.isMuted(dialoginfopage.dialog.peer.userId))
                            context.telegram.userData.removeMute(dialoginfopage.dialog.peer.userId);
                        else
                            context.telegram.userData.addMute(dialoginfopage.dialog.peer.userId);

                        return;
                    }

                    var peerid = TelegramHelper.peerId(dialoginfopage.dialog);

                    if(context.telegram.userData.isMuted(peerid))
                        context.telegram.unmute(peerid);
                    else
                        context.telegram.mute(peerid);
                }
                */
            }

            ClickableLabel
            {
                labelText: conversationTypeMessage()
                labelFont.pixelSize: Theme.fontSizeSmall
                remorseMessage: conversationTypeRemorseMessage()
                remorseRequired: true
                width: parent.width
                height: Theme.itemSizeSmall

                visible: {
                    if(dialoginfopage.chat && !dialoginfopage.chat.admin)
                        return false;

                    return true;
                }

                /* FIXME:
                onActionRequested: {
                    dialog.unreadCount = 0;

                    if(dialog.encrypted)
                        context.telegram.messagesDiscardEncryptedChat(dialog.peer.userId);
                    else
                        context.telegram.messagesDeleteHistory(TelegramHelper.peerId(dialog));

                    pageStack.pop();
                }
                */
            }

            ClickableLabel
            {
                labelText: qsTr("Add to contacts")
                labelFont.pixelSize: Theme.fontSizeSmall
                visible: !dialoginfopage.chat && !dialoginfopage.user.contact && (dialoginfopage.user.phone.length > 0)
                width: parent.width
                height: Theme.itemSizeSmall
                //FIXME: onActionRequested: context.telegram.addContact(user.firstName, user.lastName, user.phone)
            }

            Loader
            {
                id: loader

                sourceComponent: {
                    if(dialoginfopage.chat)
                        return chatinfocomponent;

                    return userinfocomponent;
                }
            }
        }
    }
}
