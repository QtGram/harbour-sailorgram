import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0
import "../items"

Page
{
    property TelegramClient telegram

    id: contactspage

    SilicaListView
    {
        anchors.fill: parent
        model: telegram.contacts
        header: PageHeader { title: qsTr("Contacts") }

        delegate: ListItem {
            width: parent.width
            height: Theme.itemSizeSmall

            ContactItem {
                id: contactitem
                anchors.fill: parent
                contactId: model.modelData.contact
                fullName: model.modelData.firstName + " " + model.modelData.lastName
                currentStatus: model.modelData.status
                contactAvatar: model.modelData.avatar
            }

            onClicked: pageStack.replace(Qt.resolvedUrl("ChatPage.qml"), { "fullName": contactitem.fullName, "currentStatus": model.modelData.status,
                                                                           "contactId": model.modelData.contact } )
        }
    }
}
