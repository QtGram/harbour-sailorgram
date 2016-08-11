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
    property bool isSecretChat

    property PeerDetails detail: PeerDetails {
        engine: context.engine
        peer: dialoginfopage.peer
    }

    id: dialoginfopage
    allowedOrientations: defaultAllowedOrientations

    function conversationTypeMessage() {
        if(isSecretChat)
            return qsTr("Delete secret chat");

        if(detail.isChat)
            return qsTr("Delete group");

        return qsTr("Delete conversation");
    }

    function conversationTypeRemorseMessage() {
        if(isSecretChat)
            return qsTr("Deleting secret chat");

        if(detail.isChat)
            return qsTr("Deleting group");

        return qsTr("Deleting conversation");
    }

    Component {
        id: userinfocomponent

        UserInfo {
            actionVisible: true
            allowSendMessage: false
            context: dialoginfopage.context
            user: detail.userFull ? detail.userFull.user: null
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
                statusText: detail.statusText
                fallbackAvatar: detail.displayName

                title: {
                    var name = detail.displayName;

                    if(detail.isUser && (detail.username.length > 0))
                        name += " (@" + detail.username + ")";

                    return name;
                }
            }

            SectionHeader { text: qsTr("Actions") }

            ClickableLabel
            {
                labelText: detail.mute ? qsTr("Enable notifications") : qsTr("Disable notifications")
                labelFont.pixelSize: Theme.fontSizeSmall
                width: parent.width
                height: Theme.itemSizeSmall

                onActionRequested: {
                    detail.mute = !detail.mute;
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
                    if(detail.isChat && !dialoginfopage.chat.editable)
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
                    if(detail.isChat || !detail.userFull || detail.userFull.user.contact || (detail.phoneNumber.length <= 0))
                        return false;

                    return true;
                }
            }

            Loader
            {
                id: loader

                sourceComponent: {
                    if(detail.isChat)
                        return chatinfocomponent;

                    return userinfocomponent;
                }
            }
        }
    }
}
