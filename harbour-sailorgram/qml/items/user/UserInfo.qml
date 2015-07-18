import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../components"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property bool actionVisible: false
    property Context context
    property User user

    id: userinfo
    height: content.height

    Column
    {
        id: content
        anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }

        SectionHeader { text: qsTr("Phone Number") }

        Label
        {
            x: Theme.paddingMedium
            width: parent.width - (x * 2)
            text: TelegramHelper.phoneNumber(user)
        }

        SectionHeader { text: qsTr("Actions"); visible: actionVisible }

        ClickableLabel
        {
            visible: actionVisible
            width: parent.width
            height: Theme.itemSizeSmall
            labelFont.pixelSize: Theme.fontSizeSmall
            labelText: qsTr("Send Message")
            onActionRequested: pageStack.replace(Qt.resolvedUrl("../../pages/conversations/ConversationPage.qml"), { "context": context, "dialog": context.telegram.fakeDialogObject(user.id, false) } )
        }

        ClickableLabel
        {
            visible: actionVisible
            width: parent.width
            height: Theme.itemSizeSmall
            labelFont.pixelSize: Theme.fontSizeSmall
            labelText: qsTr("Begin Secret Chat")

            onActionRequested: {
                context.telegram.messagesCreateEncryptedChat(user.id);

                var ancenstorpage = pageStack.previousPage(pageStack.previousPage());
                pageStack.pop(ancenstorpage);
            }
        }
    }
}
