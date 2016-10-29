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
        PullDownMenu
        {
            MenuItem
            {
                text: qsTr("About")
                onClicked: pageStack.push(Qt.resolvedUrl("../about/AboutPage.qml"), { context: contactspage.context })
            }
        }

        anchors.fill: parent
        context: contactspage.context
        clip: true

        header: PageHeader {
            title: qsTr("Contacts")
        }
    }
}
