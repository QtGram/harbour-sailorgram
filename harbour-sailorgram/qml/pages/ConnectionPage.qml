import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../models"
import "../js/Settings.js" as Settings

Page
{
    property Telegram telegram

    id: pageconnection
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if(pageconnection.status !== PageStatus.Active)
            return;

        var phonenumber = Settings.get("phonenumber");

        if(phonenumber === false) {
            pageStack.replace(Qt.resolvedUrl("PhoneNumberPage.qml"), { "telegram": pageconnection.telegram});
            return;
        }

        telegram.phoneNumber = phonenumber;
    }

    Label
    {
        anchors { bottom: indicator.top; topMargin: Theme.paddingMedium }
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
        running: true
    }

}
