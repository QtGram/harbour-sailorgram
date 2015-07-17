import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../models"

Dialog
{
    property Context context
    property var chat
    property var dialog

    id: dlgchangechattitle
    allowedOrientations: defaultAllowedOrientations
    acceptDestinationAction: PageStackAction.Pop
    canAccept: tfchattitle.text.length > 0
    onAccepted: chat && context.telegram.messagesEditChatTitle(chat.id, tfchattitle.text);

    onStatusChanged:  {
        if(dlgchangechattitle.status !== PageStatus.Active)
            return;

        dlgchangechattitle.chat = context.telegram.chat(dialog.peer.chatId);
        tfchattitle.text = chat.title;
    }

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        Column
        {
            id: content
            width: parent.width

            DialogHeader
            {
                acceptText: qsTr("Change")
            }

            TextField
            {
                id: tfchattitle
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
            }
        }
    }
}
