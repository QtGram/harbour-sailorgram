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
    allowedOrientations: Orientation.All

    onStatusChanged: {
        context.sailorgram.notifications.currentDialog = (status === PageStatus.Active) ? context.dialog : null;
    }

    MessagesModel
    {
        id: messagesmodel
        telegram: context.telegram
        dialog: dialogpage.dialog
    }

    SilicaFlickable
    {
        anchors.fill: parent

        Column
        {
            anchors.fill: parent

            DialogTopHeader
            {
                id: dialogtopheader
                title: messagesmodel.title
                statusText: messagesmodel.statusText
                peer: dialogpage.dialog
                visible: !context.chatheaderhidden
            }

            MessagesList
            {
                id: messageslist
                width: parent.width
                model: messagesmodel

                height: {
                    var h = parent.height

                    if(dialogtopheader.visible)
                        h -= dialogtopheader.height;

                    return h;
                }
            }
        }
    }
}
