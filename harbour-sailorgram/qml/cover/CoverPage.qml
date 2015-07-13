import QtQuick 2.1
import Sailfish.Silica 1.0
import "../components/cover"
import "../models"
import "../js/TelegramHelper.js" as TelegramHelper

CoverBackground
{
    property Context context

    function preparePages() /* Remove Current Chat Page, if any */
    {
        if(context.foregroundDialog === context.telegram.nullDialog)
            return;

        pageStack.pop();
        pageStack.completeAnimation();
    }

    id: cover

    Column
    {
        anchors{ fill: parent; topMargin: Theme.paddingLarge; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
        spacing: Theme.paddingLarge

        Image
        {
            id: imglogo
            source: "qrc:///res/telegramcover.png"
            opacity: 0.8
            width: parent.width / 2
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
        }

        Label
        {
            width: parent.width
            text: context.heartbeat.connected ? qsTr("Connected") : qsTr("Disconnected")
            horizontalAlignment: Text.AlignHCenter
            opacity: 0.8
        }

        Label
        {
            width: parent.width
            text: (context.telegram.unreadCount == 1) ? qsTr("%1 unread message").arg(context.telegram.unreadCount) : qsTr("%1 unread messages").arg(context.telegram.unreadCount)
            visible: context.telegram.unreadCount > 0
            wrapMode: Text.WordWrap
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryColor
        }
    }

    CoverActionList
    {
        CoverAction
        {
            iconSource: "image://theme/icon-cover-message"

            onTriggered: {
                preparePages();
                pageStack.push(Qt.resolvedUrl("../pages/users/UsersPage.qml"), { "context": context }, PageStackAction.Immediate);
                mainwindow.activate();
            }
        }
    }
}
