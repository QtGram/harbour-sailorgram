import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../components/dialog"
import "../../components/message"
import "../../components/message/panel"
import "../../model"

Page
{
    property Context context
    property var dialog

    id: dialogpage
    allowedOrientations: Orientation.All

    onStatusChanged: {
        context.sailorgram.notifications.currentDialog = (status === PageStatus.Active) ? dialogpage.dialog : null;
    }

    MessagesModel
    {
        id: messagesmodel
        telegram: context.telegram
        dialog: dialogpage.dialog
    }

    SilicaFlickable
    {
        clip: messagepanel.expanded

        anchors  {
            fill: parent
            bottomMargin: messagepanel.moving || messagepanel.expanded ? messagepanel.height : 0
        }

        Behavior on anchors.bottomMargin {
            NumberAnimation {
                duration: messagepanel.animationDuration
                easing.type: Easing.OutQuad
            }
        }

        Column
        {
            id: content
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

    MessagePanel
    {
        id: messagepanel
        width: parent.width
    }
}
