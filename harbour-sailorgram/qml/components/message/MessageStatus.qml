import QtQuick 2.1
import Sailfish.Silica 1.0

Label
{
    property color ticksColor: Theme.highlightColor
    property string messageDate
    property bool isMessageUnread
    property bool isMessageOut
    property bool isMessageEdited: false
    property bool isMute: false
    property bool dateFirst: true
    property bool dateOnly: false

    id: messagestatus
    textFormat: Text.StyledText
    verticalAlignment: Text.AlignVCenter
    font.pixelSize: Theme.fontSizeExtraSmall

    text: {
        if(dateOnly)
            return messageDate;

        var status = "";

        if(isMute)
            status += "<img align='middle' width='" + font.pixelSize + "' height='" + font.pixelSize + "' src='qrc:///res/mute.png'> ";

        if(isMessageEdited)
            status += qsTr("edited") + " ";

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
