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
            opacity: 0.5
            width: parent.width / 2
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
        }

        Label
        {
            width: parent.width
            text: !context.sailorgram.connected ? qsTr("Disconnected") : qsTr("Connected")
            horizontalAlignment: Text.AlignHCenter
            truncationMode: TruncationMode.Fade
            color: Theme.secondaryColor
            opacity: 0.7
        }

        Label
        {
            width: parent.width
            text: (context.telegram.unreadCount == 1) ? qsTr("%1 unread message").arg(context.telegram.unreadCount) : qsTr("%1 unread messages").arg(context.telegram.unreadCount)
            visible: context.telegram.unreadCount > 0
            wrapMode: Text.WordWrap
            truncationMode: TruncationMode.Fade
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryColor
            opacity: 0.5
        }
    }

    CoverActionList
    {
        enabled: pageStack.currentPage.isMediaPage !== true

        CoverAction
        {
            iconSource: "image://theme/icon-cover-message"

            onTriggered: {
                preparePages();
                pageStack.push(Qt.resolvedUrl("../pages/contacts/ContactsPage.qml"), { "context": context }, PageStackAction.Immediate);
                mainwindow.activate();
            }
        }

        CoverAction
        {
            iconSource: context.telegram.globalMute ? "image://theme/icon-cover-unmute" : "image://theme/icon-cover-mute"

            onTriggered: {
                context.telegram.globalMute = !context.telegram.globalMute;
            }
        }
    }
}
