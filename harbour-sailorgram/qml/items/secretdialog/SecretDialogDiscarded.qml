import QtQuick 2.1
import Sailfish.Silica 1.0

Label
{
    id: secretdialogdiscarded
    visible: chat && (chat.classType === TelegramConstants.typeEncryptedChatDiscarded);
    height: Theme.itemSizeSmall
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    text: qsTr("Secret chat cancelled")
}
