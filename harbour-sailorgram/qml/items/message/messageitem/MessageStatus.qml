import QtQuick 2.1
import harbour.sailorgram.TelegramQml 1.0
import "../../../js/TelegramHelper.js" as TelegramHelper

Image
{
    property Message message

    id: messagestatus
    visible: message.out && !TelegramHelper.isServiceMessage(message)
    fillMode: Image.PreserveAspectFit

    source: {
        if(!message.unread)
            return "qrc:///res/read.png";

        if(message.sent)
            return "qrc:///res/sent.png";

        return "qrc:///res/out.png";
    }
}
