import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../items/user"
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property Context context
    property Dialog dialog

    id: userspage
    allowedOrientations: defaultAllowedOrientations

    SilicaListView
    {
        anchors.fill: parent
        spacing: Theme.paddingMedium
        clip: true

        header: PageHeader {
            title: qsTr("Add Chat Member")
        }

        model: ContactsModel {
            telegram: context.telegram
        }

        delegate: ListItem {
            contentWidth: parent.width
            contentHeight: Theme.itemSizeSmall

            UserItem {
                id: useritem
                anchors.fill: parent
                context: context.telegram
                user: context.telegram.user(item.userId)
            }

            onClicked: {
                context.telegram.messagesAddChatUser(TelegramHelper.peerId(dialog), item.userId);
                pageStack.pop();
            }
        }
    }
}
