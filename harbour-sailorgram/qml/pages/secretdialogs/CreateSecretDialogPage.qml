import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0
import "../../models"
import "../../items/user"

Page
{
    property Context context

    id: createsecretdialogpage
    allowedOrientations: defaultAllowedOrientations

    SilicaFlickable
    {
        anchors.fill: parent

        PageHeader
        {
            id: pageheader
            title: qsTr("New Secret Chat")
        }

        SilicaListView
        {
            id: lvcontacts
            anchors { left: parent.left; top: pageheader.bottom; right: parent.right; bottom: parent.bottom }
            spacing: Theme.paddingMedium
            clip: true

            model: SailorgramContactsModel {
                id: contactsmodels
                engine: context.engine
            }

            delegate: ListItem {
                contentWidth: parent.width
                contentHeight: Theme.itemSizeSmall

                onClicked: {
                    //FIXME: context.telegram.messagesCreateEncryptedChat(item.userId);
                    pageStack.pop();
                }

                UserItem {
                    id: useritem
                    anchors { fill: parent; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
                    context: createsecretdialogpage.context
                    user: model.item.user
                }
            }
        }
    }
}
