import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../components"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{
    property bool allowSendMessage: true
    property bool actionVisible: false
    property bool showHeader: false
    property Context context
    property User user

    id: userinfo
    width: content.width
    height: column.height

    Column
    {
        id: column
        anchors { left: parent.left; top: parent.top; right: parent.right }

        SectionHeader
        {
            text: qsTr("Actions")
            visible: showHeader
        }

        ClickableLabel
        {
            visible: allowSendMessage && actionVisible
            width: parent.width
            height: Theme.itemSizeSmall
            labelFont.pixelSize: Theme.fontSizeSmall
            labelText: qsTr("Send Message")
            onActionRequested: pageStack.replace(Qt.resolvedUrl("../../pages/dialogs/DialogPage.qml"), { "context": context, "dialog": context.telegram.fakeDialogObject(user.id, false) } )
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

                var firstpage = pageStack.currentPage;

                for(var i = pageStack.depth; i > 1; i--)
                    firstpage = pageStack.previousPage(firstpage);

                pageStack.pop(firstpage);
            }
        }

        SectionHeader
        {
            text: qsTr("Phone Number")
            visible: user.phone.length > 0
        }

        BackgroundItem
        {
            id: phoneitem
            height: phonelabel.height + phonemenu.height

            onPressAndHold: phonemenu.show(phoneitem)

            Label
            {
                id: phonelabel
                anchors { left: parent.left; leftMargin: Theme.paddingMedium; top: parent.top; right: parent.right; rightMargin: Theme.paddingMedium }
                height: Theme.itemSizeSmall
                verticalAlignment: Text.AlignVCenter
                text: TelegramHelper.phoneNumber(user)
                visible: user.phone.length > 0
            }

            ContextMenu
            {
                id: phonemenu

                MenuItem
                {
                    text: qsTr("Copy to clipboard")

                    onClicked: Clipboard.text = phonelabel.text
                }
            }
        }
    }
}
