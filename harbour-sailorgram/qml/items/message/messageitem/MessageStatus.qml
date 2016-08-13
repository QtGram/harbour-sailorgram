import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../../models"
import "../../../components"
import "../../../js/ColorScheme.js" as ColorScheme

ResizableLabel
{
    property color ticksColor: messagestatus.color
    property bool dateFirst: true
    property bool dateOnly: false
    property Context context
    property bool isActionMessage
    property string messageDate
    property bool isMessageUnread
    property bool isMessageOut

    id: messagestatus
    visible: !isActionMessage
    color: ColorScheme.colorizeText(isActionMessage, isMessageOut, context)
    horizontalAlignment: isMessageOut ? Text.AlignLeft : Text.AlignRight
    verticalAlignment: Text.AlignVCenter
    font.pixelSize: Theme.fontSizeTiny
    textFormat: Text.StyledText

    text: {
        if(dateOnly)
            return messageDate;

        var status = "";

        if(dateFirst)
            status += messageDate + " ";

        if(isMessageOut) {
            status += "<font color=\"" + ticksColor + "\">";

            if(!isMessageUnread)
                status += " <b>✓✓</b> ";
            else
                status += " <b>✓</b> ";

            status += "</font>";
        }

        if(!dateFirst)
            status += " " + messageDate;

        return status;
    }
}
