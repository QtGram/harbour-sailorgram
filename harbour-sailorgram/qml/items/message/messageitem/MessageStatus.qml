import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../models"
import "../../../js/ColorScheme.js" as ColorScheme
import "../../../js/TelegramHelper.js" as TelegramHelper

Label
{
    property Context context
    property Message message

    id: messagestatus
    visible: message.out && !TelegramHelper.isServiceMessage(message)
    verticalAlignment: Text.AlignVCenter
    font.bold: true
    color: ColorScheme.colorizeTick(message, context)

    text: {
        if(!message.unread)
            return " ✓✓";

        return " ✓";
    }
}
