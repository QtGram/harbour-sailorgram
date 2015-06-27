import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../items/user"
import "../../menus"

Page
{
    property Telegram telegram

    id: userspage
    allowedOrientations: defaultAllowedOrientations

    SilicaListView
    {
        anchors.fill: parent
        spacing: Theme.paddingMedium
        header: PageHeader { title: qsTr("Contacts") }

        model: ContactsModel {
            telegram: userspage.telegram
        }

        delegate: ListItem {
            contentWidth: parent.width
            contentHeight: Theme.itemSizeSmall

            menu: UserMenu {
                id: usermenu
                telegram: userspage.telegram
                user: telegram.user(item.userId)
            }

            onClicked: pageStack.replace(Qt.resolvedUrl("../conversations/ConversationPage.qml"), { "telegram": userspage.telegram,  "dialog": userspage.telegram.fakeDialogObject(item.userId, false) } )

            UserItem {
                id: useritem
                anchors.fill: parent
                telegram: userspage.telegram
                user: telegram.user(item.userId)
            }
        }
    }
}
