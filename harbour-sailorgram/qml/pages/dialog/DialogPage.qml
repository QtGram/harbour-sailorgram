import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../components/dialog"
import "../../components/message/panel"
import "../../components/message"
import "../../components/custom"
import "../../model"

Page
{
    property bool firstLoad: false
    property Context context
    property var dialog

    id: dialogpage
    allowedOrientations: defaultAllowedOrientations

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

    MessagePopup { id: messagepopup }
    RemorsePopup { id: remorsepopup }

    SilicaFlickable
    {
        anchors.fill: parent

        PushUpMenu
        {
            enabled: !dialogmediapanel.expanded

            MenuItem
            {
                text: qsTr("Forward")
                visible: messageslist.selectionMode

                onClicked: {
                    var forwardpage = pageStack.push(Qt.resolvedUrl("ForwardPage.qml"), { context: dialogpage.context, fromDialog: dialogpage.dialog });

                    forwardpage.forwardRequested.connect(function(todialog) {
                        messagesmodel.forwardMessages(todialog, messageslist.getSelectionList());
                        context.openDialog(todialog, true);
                    });
                }
            }

            MenuItem
            {
                text: qsTr("Delete")
                visible: messageslist.selectionMode

                onClicked: {
                    remorsepopup.execute(qsTr("Deleting messages"), function() {
                        messagesmodel.deleteMessages(messageslist.getSelectionList());
                        messageslist.selectionMode = false;
                    });
                }
            }

            MenuItem
            {
                text: messageslist.selectionMode ? qsTr("Cancel selection") : qsTr("Select")

                onClicked: {
                    messageslist.selectionMode = !messageslist.selectionMode;

                    if(messageslist.selectionMode)
                        dialogmediapanel.hide();
                }
            }

            MenuItem
            {
                text: qsTr("Details")
                visible: !messageslist.selectionMode
                onClicked: pageStack.push(Qt.resolvedUrl("DetailsPage.qml"), { context: dialogpage.context, dialog: dialogpage.dialog })
            }
        }

        DialogTopHeader
        {
            id: dialogtopheader
            anchors { left: parent.left; top: parent.top; right: parent.right }
            title: messagesmodel.title
            statusText: messagesmodel.statusText
            peer: dialogpage.dialog
            visible: !context.chatheaderhidden && dialogpage.isPortrait
        }

        MessagesList
        {
            id: messageslist
            anchors { left: parent.left; top: dialogtopheader.bottom; right: parent.right; bottom: dialogmediapanel.top }
            model: messagesmodel
            clip: true
        }

        DialogMediaPanel
        {
            id: dialogmediapanel
            anchors { left: parent.left; bottom: parent.bottom; right: parent.right }

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
}
