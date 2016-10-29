import QtQuick 2.1
import Sailfish.Silica 1.0
import "../js/ColorScheme.js" as ColorScheme

CoverBackground
{
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
        height: unreadItem.height + statusLabel.height

        Item
        {
            id: unreadItem
            height: unreadCount.contentHeight
            width: unreadCount.contentWidth + unreadLabel.contentWidth

            Label
            {
                id: unreadCount
                text: "0"
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
            text: qsTr("Connected") //!context.sailorgram.connected ? qsTr("Disconnected") : qsTr("Connected")
            truncationMode: TruncationMode.Fade
            color: Theme.highlightColor //!context.sailorgram.connected ? ColorScheme.reverseColor(Theme.highlightColor) : Theme.highlightColor
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
            iconSource: "alarmcover.png" //context.notifications.globalMute ? "noalarmcover.png" : "alarmcover.png"

            onTriggered: {
                //context.notifications.globalMute = !context.notifications.globalMute;
            }
        }
    }
}
