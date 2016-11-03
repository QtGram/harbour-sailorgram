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
                text: "0"
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
            text: qsTr("Connected") //!context.sailorgram.connected ? qsTr("Disconnected") : qsTr("Connected")
            truncationMode: TruncationMode.Fade
            color: Theme.highlightColor //!context.sailorgram.connected ? ColorScheme.reverseColor(Theme.highlightColor) : Theme.highlightColor
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
                maxDialogs: 3
            }

            delegate: Label {
                width: colmessages.width
                elide: Text.ElideRight
                truncationMode: TruncationMode.Fade
                font.pixelSize: Theme.fontSizeSmall
                color: model.unreadCount > 0 ? Theme.highlightColor : Theme.primaryColor
                text: model.title

                onTextChanged: console.log(model.title)
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
                pageStack.push(Qt.resolvedUrl("../pages/contact/ContactsPage.qml"), { context: coverpage.context });
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
