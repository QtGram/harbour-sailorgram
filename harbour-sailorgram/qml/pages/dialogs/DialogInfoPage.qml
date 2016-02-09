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
    property bool muted: context.telegram.userData.isMuted(TelegramHelper.peerId(dialoginfopage.dialog))
    property bool actionVisible: true
    property Context context
    property Dialog dialog
    property Chat chat
    property User user

    Connections
    {
        target: context.telegram.userData

        onMuteChanged: {
            var peerid = TelegramHelper.peerId(dialog);

            if(id !== peerid)
                return;

            dialoginfopage.muted = context.telegram.userData.isMuted(id);
        }
    }

    id: dialoginfopage
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
            context: dialoginfopage.context
            user: dialoginfopage.user
        }
    }

    Component {
        id: chatinfocomponent

        ChatInfo {
            context: dialoginfopage.context
            dialog: dialoginfopage.dialog
            chat: dialoginfopage.chat
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
                dialog: dialoginfopage.dialog
                chat: dialoginfopage.chat
                user: dialoginfopage.user
                showType: false
            }

            SectionHeader { text: qsTr("Actions") }

            ClickableLabel
            {
                labelText: dialoginfopage.muted ? qsTr("Enable notifications") : qsTr("Disable notifications")
                labelFont.pixelSize: Theme.fontSizeSmall
                width: parent.width
                height: Theme.itemSizeSmall

                onActionRequested: {
                    if(dialoginfopage.dialog.encrypted) { // Secret chats are P2P
                        if(context.telegram.userData.isMuted(dialoginfopage.dialog.peer.userId)) {
                            context.telegram.userData.removeMute(dialoginfopage.dialog.peer.userId);
                            dialoginfopage.muted = false;
                        }
                        else {
                            context.telegram.userData.addMute(dialoginfopage.dialog.peer.userId);
                            dialoginfopage.muted = true;
                        }

                        return;
                    }

                    var peerid = TelegramHelper.peerId(dialoginfopage.dialog);

                    if(context.telegram.userData.isMuted(peerid)) {
                        context.telegram.unmute(peerid);
                        dialoginfopage.muted = false;
                    }
                    else {
                        context.telegram.mute(peerid);
                        dialoginfopage.muted = true;
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
