import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Model 1.0
import "../model"
import "../js/ColorScheme.js" as ColorScheme

CoverBackground
{
    property Context context

    id: coverpage

    Image
    {
        id: imgcover
        source: "qrc:///res/sailorgram-cover.png"
        asynchronous: true
        opacity: 0.1
        width: parent.width * 1.15
        anchors.horizontalCenter: parent.horizontalCenter
        fillMode: Image.PreserveAspectFit
    }

    Column
    {
        id: colstatus
        anchors { top: parent.top; left: parent.left; right: parent.right; topMargin: Theme.paddingMedium; leftMargin: Theme.paddingLarge; rightMargin: Theme.paddingSmall }
        height: unreaditem.height + statusLabel.height

        Item
        {
            id: unreaditem
            height: lblunreadcount.contentHeight
            width: lblunreadcount.contentWidth + lblunread.contentWidth

            Label
            {
                id: lblunreadcount
                text: context.telegram.unreadCount
                font.pixelSize: Theme.fontSizeHuge
                font.family: Theme.fontFamilyHeading
            }

            Label
            {
                id: lblunread
                text: qsTr("Unread\nmessage(s)")
                font.pixelSize: Theme.fontSizeExtraSmall
                font.family: Theme.fontFamilyHeading
                font.weight: Font.Light
                lineHeight: 0.8
                truncationMode: TruncationMode.Fade

                anchors {
                    left: lblunreadcount.right
                    leftMargin: Theme.paddingMedium
                    verticalCenter: lblunreadcount.verticalCenter
                }
            }
        }

        Label
        {
            id: statusLabel
            width: parent.width
            truncationMode: TruncationMode.Fade

            text: {
                if(context.telegram.syncing)
                    return qsTr("Syncing");

                return !context.telegram.connected ? qsTr("Disconnected") : qsTr("Connected")
            }

            color: !context.telegram.connected ? ColorScheme.reverseColor(Theme.highlightColor) : Theme.highlightColor
        }
    }

    Column
    {
        id: colmessages

        anchors {
            top: colstatus.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            topMargin: Theme.paddingSmall
            leftMargin: Theme.paddingLarge
        }

        Label
        {
            width: parent.width
            text: qsTr("Recent chats:")
            truncationMode: TruncationMode.Fade
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.primaryColor
        }

        Repeater {
            model: DialogsCoverModel {
                dialogsModel: context.dialogs
                maxDialogs: (context.dialogs.initializing || context.dialogs.loading) ? 0 : 3
            }

            delegate: Label {
                width: colmessages.width
                elide: Text.ElideRight
                truncationMode: TruncationMode.Fade
                font.pixelSize: Theme.fontSizeSmall
                color: model.unreadCount > 0 ? Theme.highlightColor : Theme.primaryColor
                text: model.title
            }
        }
    }

    CoverActionList
    {
        enabled: pageStack.currentPage.isMediaPage !== true

        CoverAction
        {
            iconSource: "image://theme/icon-cover-message"

            onTriggered: {
                pageStack.push(Qt.resolvedUrl("../pages/contact/ContactsPage.qml"), { context: coverpage.context }, PageStackAction.Immediate);
                mainwindow.activate();
            }
        }

        CoverAction
        {
            iconSource: context.sailorgram.notifications.mute ? "noalarmcover.png" : "alarmcover.png"

            onTriggered: {
                context.sailorgram.notifications.mute = !context.sailorgram.notifications.mute;
            }
        }
    }
}
