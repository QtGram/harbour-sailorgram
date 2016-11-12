import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../../components/contact"
import "../../../model"

Page
{
    property Context context
    property var users: null

    id: newgrouppage

    Component.onCompleted: {
        users = { };
    }

    SilicaFlickable
    {
        anchors.fill: parent

        PullDownMenu
        {
            MenuItem
            {
                text: qsTr("Create group")
                enabled: (tftitle.text.length > 0) && (users.count > 0)

                onClicked: {

                }
            }
        }

        Column
        {
            id: content
            anchors.fill: parent

            PageHeader {
                id: dlgheader
                title: qsTr("New Group")
            }

            TextField
            {
                id: tftitle
                width: parent.width
                placeholderText: qsTr("Title")
            }

            ContactsList
            {
                id: contactslist
                clip: true
                width: parent.width
                height: parent.height - tftitle.height - dlgheader.height
                context: newgrouppage.context
            }
        }
    }
}
