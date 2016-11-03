import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../components/contact"
import "../../model"

Dialog
{
    property Context context
    property var users: null

    id: newchatpage
    canAccept: tftitle.text.length > 0

    Component.onCompleted: {
        users = { };
    }

    SilicaFlickable
    {
        anchors.fill: parent

        Column
        {
            id: content
            anchors.fill: parent

            DialogHeader {
                id: dlgheader
                acceptText: qsTr("Create")
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
                context: newchatpage.context
            }
        }
    }
}
