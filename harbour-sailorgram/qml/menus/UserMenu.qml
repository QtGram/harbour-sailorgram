import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0
import "../models"

ContextMenu
{
    property Context context
    property SailorgramDialogItem sgDialogItem

    id: usermenu

    MenuItem
    {
        text: qsTr("Send Message")

        onClicked: {
            pageStack.navigateBack(PageStackAction.Immediate);
            pageStack.push(Qt.resolvedUrl("../pages/dialogs/DialogPage.qml"), { "context": usermenu.context, "sgDialogItem": sgDialogItem })
        }
    }

    MenuItem
    {
        text: qsTr("Profile")
        onClicked: pageStack.push(Qt.resolvedUrl("../pages/contacts/ContactPage.qml"), { "context": usermenu.context, "user": sgDialogItem.user })
    }
}
