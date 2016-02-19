import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../../models"
import "../../../components/search"
import "../../../items/message/forward"
import "../../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property Context context

    signal forwardRequested(int peerid, bool iscontact)

    id: fwddialogpage

    Component.onCompleted: {
        context.dialogs.stopUpdating = true;
    }

    Component.onDestruction: {
        context.dialogs.stopUpdating = false;
    }

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        if(!canNavigateForward) {
            var fwdcontactpage = pageStack.pushAttached(Qt.resolvedUrl("ForwardContactPage.qml"), { "context": fwddialogpage.context });
            fwdcontactpage.forwardRequested.connect(forwardRequested);
        }

        gvdialogs.model = context.dialogs;
    }

    SilicaFlickable
    {
        anchors.fill: parent

        PageHeader
        {
            id: pageheader
            title: qsTr("Forward to chat")
        }

        SilicaGridView
        {
            id: gvdialogs
            anchors { left: parent.left; top: pageheader.bottom; right: parent.right; bottom: parent.bottom; leftMargin: Theme.paddingMedium }
            cellWidth: Theme.iconSizeExtraLarge
            cellHeight: Theme.iconSizeExtraLarge
            clip: true

            delegate: ForwardDialogItem {
                width: gvdialogs.cellWidth - Theme.paddingSmall
                height: gvdialogs.cellHeight - Theme.paddingSmall
                context: fwddialogpage.context
                dialog: item
                onClicked: forwardRequested(TelegramHelper.peerId(item), false)
            }
        }
    }
}
