import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../models"
import "../../components"
import "../../items/peer"
import "../../items/user"
import "../../items/chat"

Page
{
    property bool actionVisible: true
    property Context context
    property SailorgramDialogItem sgDialogItem

    id: dialoginfopage
    allowedOrientations: defaultAllowedOrientations

    function conversationTypeMessage() {
        if(sgDialogItem.isSecretChat)
            return qsTr("Delete secret chat");

        if(sgDialogItem.isBroadcast)
            return qsTr("Delete channel");

        if(sgDialogItem.isChat)
            return qsTr("Delete group");

        return qsTr("Delete conversation");
    }

    function conversationTypeRemorseMessage() {
        if(sgDialogItem.isSecretChat)
            return qsTr("Deleting secret chat");

        if(sgDialogItem.isBroadcast)
            return qsTr("Deleting channel");

        if(sgDialogItem.isChat)
            return qsTr("Deleting group");

        return qsTr("Deleting conversation");
    }

    Component {
        id: userinfocomponent

        UserInfo {
            actionVisible: true
            allowSendMessage: false
            context: dialoginfopage.context
            user: sgDialogItem.user
        }
    }

    Component {
        id: chatinfocomponent

        ChatInfo {
            context: dialoginfopage.context
            chat: sgDialogItem.chat
            peer: sgDialogItem.peer
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
                peer: sgDialogItem.peer
                isChat: sgDialogItem.isChat
                isSecretChat: sgDialogItem.isSecretChat
                statusText: sgDialogItem.statusText
                fallbackAvatar: sgDialogItem.title

                title: {
                    var name = sgDialogItem.title;

                    if(sgDialogItem.isUser && (sgDialogItem.user.username.length > 0))
                        name += " (@" + sgDialogItem.user.username + ")";

                    return name;
                }
            }

            SectionHeader { text: qsTr("Actions") }

            ClickableLabel
            {
                labelText: sgDialogItem.mute ? qsTr("Enable notifications") : qsTr("Disable notifications")
                labelFont.pixelSize: Theme.fontSizeSmall
                width: parent.width
                height: Theme.itemSizeSmall

                onActionRequested: {
                    //FIXME: detail.mute = !detail.mute;
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

                visible: {
                    if(sgDialogItem.isChat && !sgDialogItem.chat.editable)
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
                width: parent.width
                height: Theme.itemSizeSmall
                //FIXME: onActionRequested: context.telegram.addContact(user.firstName, user.lastName, user.phone)

                visible: {
                    if(sgDialogItem.isChat || !sgDialogItem.isUser || sgDialogItem.user.contact || (sgDialogItem.user.phone.length <=0))
                        return false;

                    return true;
                }
            }

            Loader
            {
                id: loader

                sourceComponent: {
                    if(sgDialogItem.isChat)
                        return chatinfocomponent;

                    return userinfocomponent;
                }
            }
        }
    }
}
