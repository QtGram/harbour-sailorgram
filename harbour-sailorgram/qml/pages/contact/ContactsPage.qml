import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../components/contact"
import "../../model"

Page
{
    property Context context

    signal dialogCreated(var dialog)

    id: contactspage
    allowedOrientations: Orientation.All

    ContactsList
    {
        anchors.fill: parent
        context: contactspage.context
        clip: true

        contactsModel {
            onDialogCreated: contactspage.dialogCreated(dialog)
        }

        header: PageHeader {
            title: qsTr("Contacts")
        }
    }
}
