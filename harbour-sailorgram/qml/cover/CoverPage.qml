import QtQuick 2.1
import Sailfish.Silica 1.0
import "../components/cover"
import "../models"
import "../js/TelegramHelper.js" as TelegramHelper

CoverBackground
{
    property Settings settings

    function preparePages() /* Remove Current Chat Page, if any */
    {
        if(settings.foregroundDialog === settings.telegram.nullDialog)
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
                if(settings.telegram.online)
                    return qsTr("Online");

                return qsTr("Offline");
            }

            valueColor: {
                if(settings.telegram.online)
                    return "lime";

                return "red";
            }
        }

        CoverLabel
        {
            id: clunreadmsgs
            descriptionText: qsTr("Unread Messages:")
            valueText: settings.telegram.unreadCount
            valueColor: settings.telegram.unreadCount > 0 ? "yellow" : "lime"
        }

        /*
        CoverLabel
        {
            id: cllastmessageuser
            //visible: settings.lastMessage !== settings.telegram.nullMessage
            descriptionText: qsTr("Last Message By:")
            valueColor: Theme.secondaryHighlightColor

            valueText: {
                if(settings.lastMessage === settings.telegram.nullMessage)
                    return "";

                var user = settings.telegram.user(settings.lastMessage.fromId);
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
                pageStack.push(Qt.resolvedUrl("../pages/users/UsersPage.qml"), { "settings": settings, "telegram": settings.telegram }, PageStackAction.Immediate);
                mainwindow.activate();
            }
        }
    }
}
