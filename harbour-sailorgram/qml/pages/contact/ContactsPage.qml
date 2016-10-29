import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../components/contact"
import "../../model"

Page
{
    property Context context

    id: contactspage

    ContactsList
    {
        anchors.fill: parent
        context: contactspage.context
        clip: true

        header: PageHeader {
            title: qsTr("Contacts")
        }
    }
}
