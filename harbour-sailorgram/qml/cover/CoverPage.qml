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
        anchors{ fill: parent; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
        spacing: Theme.paddingMedium

        Image
        {
            id: imglogo
            source: "qrc:///res/telegramcover.png"
            width: parent.width / 2
            anchors { horizontalCenter: parent.horizontalCenter }
            fillMode: Image.PreserveAspectFit
        }

        CoverLabel
        {
            id: clstatus
            descriptionText: qsTr("Status:")

            valueText: {
                if(context.heartbeat.connected)
                    return qsTr("Online");

                return qsTr("Offline");
            }

            valueColor: {
                if(context.heartbeat.connected)
                    return "lime";

                return "red";
            }
        }

        CoverLabel
        {
            id: clunreadmsgs
            descriptionText: qsTr("Unread Messages:")
            valueText: context.telegram.unreadCount
            valueColor: context.telegram.unreadCount > 0 ? "yellow" : "lime"
        }

        /*
        CoverLabel
        {
            id: cllastmessageuser
            //visible: context.lastMessage !== context.telegram.nullMessage
            descriptionText: qsTr("Last Message By:")
            valueColor: Theme.secondaryHighlightColor

            valueText: {
                if(context.lastMessage === context.telegram.nullMessage)
                    return "";

                var user = context.telegram.user(context.lastMessage.fromId);
                return TelegramHelper.userName(user);
            }
        }
        */
    }

    CoverActionList
    {
        CoverAction
        {
            iconSource: "image://theme/icon-cover-message"

            onTriggered: {
                preparePages();
                pageStack.push(Qt.resolvedUrl("../pages/users/UsersPage.qml"), { "context": context, "telegram": context.telegram }, PageStackAction.Immediate);
                mainwindow.activate();
            }
        }
    }
}
