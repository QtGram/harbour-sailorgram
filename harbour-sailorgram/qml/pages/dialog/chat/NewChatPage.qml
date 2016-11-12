import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../../components/custom"
import "../../../model"

Page
{
    property Context context

    signal cloudChatRequested();

    id: newchatpage

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        Column
        {
            id: content
            width: parent.width

            PageHeader { title: qsTr("New Chat") }

            ClickableLabel
            {
                text: qsTr("Create a Group")
                icon: "qrc:///res/chat.png"
                width: parent.width
                height: Theme.itemSizeSmall
                onActionRequested: pageStack.replace(Qt.resolvedUrl("NewGroupPage.qml"), { context: newchatpage.context })
            }

            ClickableLabel
            {
                text: qsTr("Create a Channel")
                icon: "qrc:///res/channel.png"
                width: parent.width
                height: Theme.itemSizeSmall
                onActionRequested: pageStack.replace(Qt.resolvedUrl("NewChannelPage.qml"), { context: newchatpage.context })
            }

            ClickableLabel
            {
                text: qsTr("Cloud Chat")
                icon: "qrc:///res/cloud.png"
                width: parent.width
                height: Theme.itemSizeSmall
                onActionRequested: cloudChatRequested()
            }
        }
    }
}
