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
                user: telegram.user(item.userId)
            }

            onClicked: pageStack.replace(Qt.resolvedUrl("ChatPage.qml"), { "fullName": contactitem.fullName, "currentStatus": model.modelData.status,
                                                                           "contactId": model.modelData.contact } )
        }
    }
}
