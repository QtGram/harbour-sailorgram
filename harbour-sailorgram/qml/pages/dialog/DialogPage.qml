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

    RemorsePopup { id: remorsepopup }

    SilicaFlickable
    {
        anchors  {
            left: parent.left
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            bottomMargin: dialogmediapanel.height
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
                    var h = parent.height;

                    if(dialogtopheader.visible)
                        h -= dialogtopheader.height;

                    return h;
                }
            }
        }
    }

    DialogMediaPanel
    {
        id: dialogmediapanel
        anchors { left: parent.left; bottom: parent.bottom; right: parent.right }
        width: parent.width

        onShareImage: {
            var imageselector = pageStack.push(Qt.resolvedUrl("../../pages/selector/SelectorImagePage.qml"), { context: dialogpage.context });
            imageselector.imageSelected.connect(function(image) {
                messagesmodel.sendPhoto(image, "");
                pageStack.pop(dialogpage);
            });
        }

        onShareFile: {
            var fileselector = pageStack.push(Qt.resolvedUrl("../../pages/selector/SelectorFilePage.qml"), { context: dialogpage.context });

            fileselector.fileSelected.connect(function(file)  {
                messagesmodel.sendFile(file, "");
                pageStack.pop(dialogpage);
            });
        }

        onShareLocation: {
            remorsepopup.execute(qsTr("Sending location"), function() {
                if(dialogpage.context.positionSource.valid) {
                    messagesmodel.sendLocation(dialogpage.context.positionSource.position.coordinate.latitude,
                                               dialogpage.context.positionSource.position.coordinate.longitude);
                    return;
                }

                messageslist.positionPending = true;
                dialogpage.context.positionSource.update();
            });
        }
    }
}
