import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../items/message"
import "../../items/peer"
import "../../js/TelegramHelper.js" as TelegramHelper

SilicaGridView
{
    property Context context
    property Message message

    signal messageForwarded()

    id: messagefwdgridview
    flow: GridView.TopToBottom
    flickableDirection: Flickable.HorizontalFlick
    model: message ? context.dialogs : null
    cellWidth: Theme.iconSizeLarge
    cellHeight: Theme.iconSizeLarge
    height: message ? (cellHeight * 3) : 0
    visible: message !== null
    clip: true

    delegate: ForwardMessageItem {
        width: cellWidth - Theme.paddingSmall
        height: cellHeight - Theme.paddingSmall
        context: messagefwdgridview.context
        dialog: item

        onClicked: {
            context.telegram.forwardMessages([message.id], TelegramHelper.peerId(item));
            messagefwdgridview.message = null;
            messageForwarded();
        }
    }
}
