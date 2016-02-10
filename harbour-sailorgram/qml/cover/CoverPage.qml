import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import harbour.sailorgram.Model 1.0
import "../components/cover"
import "../models"
import "../js/TelegramHelper.js" as TelegramHelper
import "../js/ColorScheme.js" as ColorScheme

CoverBackground
{
    property Context context

    function preparePages() /* Remove Current Chat Page, if any */
    {
        if(!context.sailorgram.foregroundDialog)
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
        id: statusColumn
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
                text: context.telegram.unreadCount
                font.pixelSize: Theme.fontSizeHuge
                font.family: Theme.fontFamilyHeading
            }

            Label
            {
                id: unreadLabel
                text: (context.telegram.unreadCount == 1) ? qsTr("Unread\nmessage") : qsTr("Unread\nmessages")
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
            top: statusColumn.bottom
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
                maxDialogs: 3
            }

            delegate: Label {
                width: messagesColumn.width
                elide: Text.ElideRight
                truncationMode: TruncationMode.Fade
                font.pixelSize: Theme.fontSizeSmall
                color: item.unreadCount > 0 ? Theme.highlightColor : Theme.primaryColor

                text: {
                    if(TelegramHelper.isChat(item)) {
                        var chat = context.telegram.chat(item.peer.chatId);
                        return (item.unreadCount > 0) ? (item.unreadCount + " " + chat.title) : chat.title;
                    }

                    var user = context.telegram.user(item.peer.userId);
                    return (item.unreadCount > 0) ? (item.unreadCount + " "  + TelegramHelper.completeName(user)) : TelegramHelper.completeName(user);
                }
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
            iconSource: context.telegram.globalMute ? "noalarmcover.png" : "alarmcover.png"

            onTriggered: {
                context.telegram.globalMute = !context.telegram.globalMute;
            }
        }
    }
}
