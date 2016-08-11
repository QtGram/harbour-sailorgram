import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0
import "../components/cover"
import "../models"
import "../js/TelegramHelper.js" as TelegramHelper
import "../js/ColorScheme.js" as ColorScheme

CoverBackground
{
    property Context context

    function preparePages() /* Remove Current Chat Page, if any */
    {
        if(!context.sailorgram.currentPeerKey)
            return;

        pageStack.pop();
        pageStack.completeAnimation();
    }

    id: cover

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
        height: unreadItem.height + statusLabel.height

        Item
        {
            id: unreadItem
            height: unreadCount.contentHeight
            width: unreadCount.contentWidth + unreadLabel.contentWidth

            Label
            {
                id: unreadCount
                text: context.notifications.globalUnreadCount
                font.pixelSize: Theme.fontSizeHuge
                font.family: Theme.fontFamilyHeading
            }

            Label
            {
                id: unreadLabel
                text: qsTr("Unread\nmessage(s)")
                font.pixelSize: Theme.fontSizeExtraSmall
                font.family: Theme.fontFamilyHeading
                font.weight: Font.Light
                lineHeight: 0.8
                truncationMode: TruncationMode.Fade

                anchors {
                    left: unreadCount.right
                    leftMargin: Theme.paddingMedium
                    verticalCenter: unreadCount.verticalCenter
                }
            }
        }

        Label
        {
            id: statusLabel
            width: parent.width
            text: !context.sailorgram.connected ? qsTr("Disconnected") : qsTr("Connected")
            truncationMode: TruncationMode.Fade
            color: !context.sailorgram.connected ? ColorScheme.reverseColor(Theme.highlightColor) : Theme.highlightColor
        }
    }

    Column
    {
        id: messagesColumn

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
            model: SailorgramDialogsCoverModel {
                dialogsModel: context.dialogs
                maxDialogs: 3
            }

            delegate: Label {
                width: messagesColumn.width
                elide: Text.ElideRight
                truncationMode: TruncationMode.Fade
                font.pixelSize: Theme.fontSizeSmall
                color: (model.item.unreadCount > 0) ? Theme.highlightColor : Theme.primaryColor
                text: (model.item.unreadCount > 0) ? (model.item.unreadCount + " " + model.item.title) : model.item.title;
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
                preparePages();
                pageStack.pop(null, PageStackAction.Immediate);
                pageStack.navigateForward(PageStackAction.Immediate);
                mainwindow.activate();
            }
        }

        CoverAction
        {
            iconSource: context.sailorgram.globalMute ? "noalarmcover.png" : "alarmcover.png"

            onTriggered: {
                context.sailorgram.globalMute = !context.sailorgram.globalMute;
            }
        }
    }
}
