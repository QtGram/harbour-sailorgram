import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Telegram 1.0

Label
{
    property SailorgramDialogItem sgDialogItem

    id: secretdialogdiscarded
    visible: sgDialogItem.secretChatState === SailorgramEnums.SecretChatStateCancelled
    height: Theme.itemSizeSmall
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    text: qsTr("Secret chat cancelled")
}
