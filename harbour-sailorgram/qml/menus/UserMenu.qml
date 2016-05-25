import QtQuick 2.1
import Sailfish.Silica 1.0
import "../models"

ContextMenu
{
    property Context context
    property var dialogModelItem

    id: usermenu

    MenuItem
    {
        text: qsTr("Send Message")

        onClicked: {
            pageStack.navigateBack(PageStackAction.Immediate);
            pageStack.push(Qt.resolvedUrl("../pages/dialogs/DialogPage.qml"), { "context": usermenu.context, "dialogModelItem": dialogItemModel })
        }
    }

    MenuItem
    {
        text: qsTr("Profile")
        onClicked: pageStack.push(Qt.resolvedUrl("../pages/contacts/ContactPage.qml"), {"context": usermenu.context, "user": usermenu.user })
    }
}
