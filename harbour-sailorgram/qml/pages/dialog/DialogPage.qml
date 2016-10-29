import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../components/dialog"
import "../../components/message"
import "../../model"

Page
{
    property Context context
    property var dialog

    id: dialogpage

    MessagesModel
    {
        id: messagesmodel
        telegram: context.telegram
        dialog: dialogpage.dialog
    }

    Column
    {
        anchors.fill: parent

        DialogTopHeader
        {
            id: dialogtopheader
            title: messagesmodel.title
            statusText: messagesmodel.statusText
            peer: dialogpage.dialog
        }

        MessagesList
        {
            id: messageslist
            width: parent.width
            height: parent.height - dialogtopheader.height
            model: messagesmodel
        }
    }
}
