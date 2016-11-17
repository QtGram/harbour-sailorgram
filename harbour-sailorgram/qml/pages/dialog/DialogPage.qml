import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../components/dialog"
import "../../components/message"
import "../../components/message/panel"
import "../../model"

Page
{
    property bool firstLoad: false
    property Context context
    property var dialog

    id: dialogpage
    allowedOrientations: Orientation.All

    onStatusChanged: {
        context.sailorgram.notifications.currentDialog = (status === PageStatus.Active) ? dialogpage.dialog : null;

        if((status !== PageStatus.Active) || !firstLoad)
            return;

        messagesmodel.dialog = dialog;
        firstLoad = false;
    }

    MessagesModel
    {
        id: messagesmodel
        telegram: context.telegram
        isActive: (Qt.application.state === Qt.ApplicationActive) && (dialogpage.status === PageStatus.Active)
    }

    SilicaFlickable
    {
        anchors  {
            left: parent.left
            top: parent.top
            right: parent.right
            bottom: parent.bottom
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
                visible: !context.chatheaderhidden && dialogpage.isPortrait
            }

            MessagesList
            {
                id: messageslist
                model: messagesmodel
                width: parent.width
                clip: true

                height: {
                    var h = parent.height - mediaPanelHeight;

                    if(dialogtopheader.visible)
                        h -= dialogtopheader.height;

                    return h;
                }
            }
        }
    }
}
