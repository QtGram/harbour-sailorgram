import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../items/user"
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property Settings settings
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
            telegram: settings.telegram
        }

        delegate: ListItem {
            contentWidth: parent.width
            contentHeight: Theme.itemSizeSmall

            UserItem {
                id: useritem
                anchors.fill: parent
                telegram: settings.telegram
                user: settings.telegram.user(item.userId)
            }

            onClicked: {
                settings.telegram.messagesAddChatUser(TelegramHelper.peerId(dialog.peer), item.userId);
                pageStack.pop();
            }
        }
    }
}
