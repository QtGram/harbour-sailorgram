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
    property color ticksColor: messagestatus.color
    property bool dateFirst: true
    property bool dateOnly: false

    id: messagestatus
    visible: !TelegramHelper.isServiceMessage(message)
    color: ColorScheme.colorizeText(message, context)
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: message.out ? Text.AlignLeft : Text.AlignRight
    textFormat: Text.StyledText
    font.pixelSize: Theme.fontSizeTiny

    text: {
        if(dateOnly)
            return TelegramHelper.printableDate(message.date);

        var status = "";

        if(dateFirst)
            status += TelegramHelper.printableDate(message.date) + " ";

        if(message.out)
        {
            status += "<font color=\"" + ticksColor + "\">";

            if(!message.unread)
                status += " <b>✓✓</b> ";
            else
                status += " <b>✓</b> ";

            status += "</font>";
        }

        if(!dateFirst)
            status += " " + TelegramHelper.printableDate(message.date);

        return status;
    }
}
