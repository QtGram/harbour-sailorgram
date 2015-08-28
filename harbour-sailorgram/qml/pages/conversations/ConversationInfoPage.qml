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
    property bool actionVisible: true
    property Context context
    property Dialog dialog
    property Chat chat
    property User user

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
                labelText: context.telegram.userData.isMuted(TelegramHelper.conversationId(conversationinfopage.dialog, conversationinfopage.context)) ? qsTr("Disable Notifications") : qsTr("Enable Notifications")
                labelFont.pixelSize: Theme.fontSizeSmall
                width: parent.width
                height: Theme.itemSizeSmall

                onActionRequested: {
                    var conversationid = TelegramHelper.conversationId(conversationinfopage.dialog, conversationinfopage.context);

                    if(context.telegram.userData.isMuted(conversationid))
                        context.telegram.unmute(conversationid);
                    else
                        context.telegram.mute(conversationid);
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
