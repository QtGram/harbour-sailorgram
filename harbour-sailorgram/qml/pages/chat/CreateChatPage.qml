import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Model 1.0
import harbour.sailorgram.TelegramQml 2.0 as Telegram
import "../../models"
import "../../items/user"
import "../../js/TelegramHelper.js" as TelegramHelper

Dialog
{
    property Context context
    property int count: 0
    property var users

    id: dlgcreatechat
    allowedOrientations: defaultAllowedOrientations
    acceptDestinationAction: PageStackAction.Pop
    canAccept: (count > 0) && (tfgroupname.text.length > 0)

    Component.onCompleted: {
        dlgcreatechat.users = new Object;
    }

    onAccepted: {
        var userlist = new Array;

        for(var prop in dlgcreatechat.users)
            userlist.push(dlgcreatechat.users[prop]);

        contactsmodels.createChat(userlist, tfgroupname.text);
    }

    SilicaFlickable
    {
        anchors.fill: parent

        DialogHeader
        {
            id: dlgheader
            acceptText: qsTr("New Group")
        }

        TextField
        {
            id: tfgroupname
            anchors { left: parent.left; top: dlgheader.bottom; right: parent.right }
            placeholderText: qsTr("Group Name")
        }

        SilicaListView
        {
            id: lvcontacts
            anchors { left: parent.left; top: tfgroupname.bottom; right: parent.right; bottom: parent.bottom }
            spacing: Theme.paddingMedium
            clip: true

            model: Telegram.DialogListModel {
                id: contactsmodels
                engine: context.engine
                visibility: Telegram.DialogListModel.VisibilityContacts | Telegram.DialogListModel.VisibilityEmptyDialogs
                sortFlag: [ Telegram.DialogListModel.SortByName, Telegram.DialogListModel.SortByOnline ]
            }

            delegate: ListItem {
                contentWidth: parent.width
                contentHeight: Theme.itemSizeSmall

                onClicked: {
                    swselectcontact.checked = !swselectcontact.checked;
                }

                UserItem {
                    id: useritem
                    anchors { left: parent.left; top: parent.top; right: swselectcontact.right; bottom: parent.bottom; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
                    context: dlgcreatechat.context
                    user: model.user
                }

                Switch {
                    id: swselectcontact
                    anchors { right: parent.right; top: parent.top; bottom: parent.bottom }
                    width: parent.height

                    onCheckedChanged: {
                        checked ? count++ : count--

                        if(checked)
                            users[index] = model.user;
                        else
                            delete users[index];
                    }
                }
            }
        }
    }
}
