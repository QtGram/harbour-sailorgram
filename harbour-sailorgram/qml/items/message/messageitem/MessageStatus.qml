import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../../js/TelegramHelper.js" as TelegramHelper

Label
{
    property Message message

    id: messagestatus
    visible: message.out && !TelegramHelper.isServiceMessage(message)
    verticalAlignment: Text.AlignVCenter
    font.bold: true

    color: {
        if(!message.sent)
            return "gray";

        return Theme.highlightDimmerColor;
    }

    text: {
        if(!message.unread)
            return " ✓✓";

        return " ✓";
    }
}
