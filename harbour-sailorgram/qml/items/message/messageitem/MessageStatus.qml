import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../../models"
import "../../../components"
import "../../../js/ColorScheme.js" as ColorScheme
import "../../../js/TelegramHelper.js" as TelegramHelper

ResizableLabel
{
    property color ticksColor: messagestatus.color
    property bool dateFirst: true
    property bool dateOnly: false
    property Context context
    property string messageDateTime
    property bool messageUnread
    property bool messageType
    property bool messageOut

    id: messagestatus
    visible: messageType !== Enums.TypeActionMessage
    color: ColorScheme.colorizeText(messageType, messageOut, context)
    horizontalAlignment: messageOut ? Text.AlignLeft : Text.AlignRight
    verticalAlignment: Text.AlignVCenter
    font.pixelSize: Theme.fontSizeTiny
    textFormat: Text.StyledText

    text: {
        if(dateOnly)
            return messageDateTime;

        var status = "";

        if(dateFirst)
            status += messageDateTime + " ";

        if(messageOut) {
            status += "<font color=\"" + ticksColor + "\">";

            if(!messageUnread)
                status += " <b>✓✓</b> ";
            else
                status += " <b>✓</b> ";

            status += "</font>";
        }

        if(!dateFirst)
            status += " " + messageDateTime;

        return status;
    }
}
