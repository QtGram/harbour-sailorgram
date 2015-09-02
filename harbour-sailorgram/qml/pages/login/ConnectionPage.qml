import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../components"
import "../../js/Settings.js" as Settings

Page
{
    property Context context
    property bool initialized: false

    id: connectionpage
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if(forwardNavigation || (connectionpage.status !== PageStatus.Active))
            return;

        var phonenumber = Settings.get("phonenumber");

        if(phonenumber === false) {
            pageStack.replace(Qt.resolvedUrl("PhoneNumberPage.qml"), { "context": connectionpage.context });
            return;
        }

        timlogin.restart();
        timdisplaystatus.restart();
        context.telegram.phoneNumber = phonenumber;
    }

    Timer
    {
        id: timlogin
        interval: 10000 // 10s
        running: true
    }

    Timer
    {
        id: timdisplaystatus
        interval: context.heartbeat.interval / 2
        running: true
    }

    Row
    {
        anchors { top: parent.top; right: parent.right }
        height: csitem.height

        Label
        {
            text: qsTr("Telegram Status")
            anchors.verticalCenter: csitem.verticalCenter
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignLeft
            font.family: Theme.fontFamilyHeading
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.highlightColor
        }

        ConnectionStatus
        {
            id: csitem
            context: connectionpage.context
            forceActive: timdisplaystatus.running
        }
    }

    Label
    {
        anchors { bottom: indicator.top; bottomMargin: Theme.paddingMedium }
        width: parent.width
        font.pixelSize: Theme.fontSizeExtraLarge
        horizontalAlignment: Text.AlignHCenter
        color: Theme.secondaryHighlightColor
        text: qsTr("Connecting")
    }

    BusyIndicator
    {
        id: indicator
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
        running: true
    }

    Row
    {
        anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom; bottomMargin: Theme.paddingLarge }

        Button
        {
            text: qsTr("Login Again")
            visible: !timlogin.running

            onClicked: {
                if(context.telegram.phoneNumber.length > 0) {
                    pageStack.replace(Qt.resolvedUrl("AuthorizationPage.qml"), { "context": connectionpage.context, "resendCode": true });
                }
                else
                    pageStack.replace(Qt.resolvedUrl("PhoneNumberPage.qml"), { "context": connectionpage.context });
            }
        }

        Button
        {
            text: qsTr("Error log")
            visible: !timlogin.running

            onClicked: {
                pageStack.pushAttached(Qt.resolvedUrl("../settings/DebugSettingsPage.qml"), { "context": connectionpage.context });
                pageStack.navigateForward(PageStackAction.Animated);
            }
        }
    }
}
