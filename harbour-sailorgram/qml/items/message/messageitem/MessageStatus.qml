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
    visible: !TelegramHelper.isServiceMessage(message)
    color: ColorScheme.colorize(message, context)
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: message.out ? Text.AlignLeft : Text.AlignRight
    textFormat: Text.StyledText
    font.pixelSize: Theme.fontSizeTiny

    text: {
        var status = TelegramHelper.printableDate(message.date);

        if(message.out)
        {
            if(!message.unread)
                status += "  <b>✓✓</b>";
            else
                status += "  <b>✓</b>";
        }

        return " " + status;
    }
}
