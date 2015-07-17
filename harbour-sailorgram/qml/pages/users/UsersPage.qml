import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../items/user"
import "../../menus"

Page
{
    property Context context

    id: userspage
    allowedOrientations: defaultAllowedOrientations

    SilicaListView
    {
        anchors.fill: parent
        spacing: Theme.paddingMedium
        header: PageHeader { title: qsTr("Contacts") }

        model: ContactsModel {
            telegram: userspage.context.telegram
        }

        delegate: ListItem {
            contentWidth: parent.width
            contentHeight: Theme.itemSizeSmall

            menu: UserMenu {
                id: usermenu
                context: userspage.context
                user: context.telegram.user(item.userId)
            }

            onClicked: pageStack.replace(Qt.resolvedUrl("../conversations/ConversationPage.qml"), { "context": userspage.context, "dialog": context.telegram.fakeDialogObject(item.userId, false) } )

            UserItem {
                id: useritem
                anchors.fill: parent
                context: userspage.context
                user: context.telegram.user(item.userId)
            }
        }
    }
}
