import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../items/user"
import "../../menus"

Page
{
    property Context context

    id: contactspage
    allowedOrientations: defaultAllowedOrientations

    SilicaListView
    {
        PullDownMenu
        {
            property Context context

            id: userspulldownmenu

            MenuItem
            {
                text: qsTr("Add Contact")
                onClicked: pageStack.push(Qt.resolvedUrl("AddContactPage.qml"), { "context": context });
            }
        }

        anchors.fill: parent
        spacing: Theme.paddingMedium
        header: PageHeader { title: qsTr("Contacts") }
        model: context.contacts

        delegate: ListItem {
            contentWidth: parent.width
            contentHeight: Theme.itemSizeSmall

            menu: UserMenu {
                id: usermenu
                context: contactspage.context
                user: context.telegram.user(item.userId)
            }

            onClicked: pageStack.replace(Qt.resolvedUrl("../dialogs/DialogPage.qml"), { "context": contactspage.context, "dialog": context.telegram.fakeDialogObject(item.userId, false) } )

            UserItem {
                id: useritem
                anchors.fill: parent
                context: contactspage.context
                user: context.telegram.user(item.userId)
            }
        }
    }
}
