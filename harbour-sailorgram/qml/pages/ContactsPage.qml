import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../items"

Page
{
    property Telegram telegram

    id: contactspage

    SilicaListView
    {
        anchors.fill: parent
        spacing: Theme.paddingMedium
        header: PageHeader { title: qsTr("Contacts") }

        model: ContactsModel {
            telegram: contactspage.telegram
        }

        delegate: ListItem {
            width: parent.width
            height: Theme.itemSizeSmall

            ContactItem {
                id: contactitem
                anchors.fill: parent
                telegram: contactspage.telegram
                user: telegram.user(item.userId)
            }

            onClicked: pageStack.replace(Qt.resolvedUrl("ConversationPage.qml"), { "telegram": contactspage.telegram,  "dialog": contactspage.telegram.fakeDialogObject(item.userId, false) } )
        }
    }
}
