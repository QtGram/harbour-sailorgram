import QtQuick 2.1
import Sailfish.Silica 1.0
import "../components"
import "../items"

Page
{
    property string fullName
    property string currentStatus
    property string contactId

    id: chatpage

    SilicaListView
    {
        PullDownMenu {
            id: menu

            MenuItem {
                text: qsTr("Delete History")
            }

            MenuItem {
                text: qsTr("Remove Chat")
            }

            MenuItem {
                text: qsTr("Silence Notifications")
            }
        }

        id: lvchat
        anchors { left: parent.left; top: parent.top; right: parent.right; bottom: messagebar.top }
        clip: true

        model: 10

        delegate: ChatItem {
            text: "Testo " + index;
            me: index % 2
        }

        header: PageHeader {
            title: qsTr("Chatting with: ") + fullName
        }
    }

    MessageBar
    {
        id: messagebar
        anchors { left: parent.left; bottom: parent.bottom; right: parent.right }
    }
}
