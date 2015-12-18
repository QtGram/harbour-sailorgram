import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/TelegramConstants.js" as TelegramConstants

Item
{
    property User user
    property EncryptedChat chat

    id: secretdialogwaiting
    visible: user && chat && (chat.classType === TelegramConstants.typeEncryptedChatWaiting)
    width: busyindicator.width + lblwaiting.contentWidth + Theme.paddingMedium

    BusyIndicator
    {
        id: busyindicator
        size: BusyIndicatorSize.Small
        anchors { left: parent.left; verticalCenter: lblwaiting.verticalCenter }
        running: secretdialogwaiting.visible
    }

    Label
    {
        id: lblwaiting
        text: qsTr("Waiting for %1 to get online").arg(TelegramHelper.completeName(user))
        anchors { left: busyindicator.right; top: parent.top; bottom: parent.bottom; leftMargin: Theme.paddingMedium }
        color: Theme.secondaryHighlightColor
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        wrapMode: Text.NoWrap
        elide: Text.ElideRight
    }
}
