import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../js/TelegramConstants.js" as TelegramConstants

Item
{
    property EncryptedChat chat

    id: secretchatdiscarded
    visible: chat && (chat.classType === TelegramConstants.typeEncryptedChatDiscarded);

    Label
    {
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: qsTr("Secret chat cancelled")
    }
}
