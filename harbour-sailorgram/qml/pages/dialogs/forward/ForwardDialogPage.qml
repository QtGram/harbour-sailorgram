import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../../models"
import "../../../components/search"
import "../../../items/message/forward"
import "../../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property Context context

    signal forwardRequested(var dialogmodelitem)

    id: fwddialogpage

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        if(!canNavigateForward) {
            var fwdcontactpage = pageStack.pushAttached(Qt.resolvedUrl("ForwardContactPage.qml"), { "context": fwddialogpage.context });
            fwdcontactpage.forwardRequested.connect(forwardRequested);
        }
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
            model: context.dialogs
            clip: true

            delegate: ForwardDialogItem { // NOTE: FDI Deprecated
                width: gvdialogs.cellWidth - Theme.paddingSmall
                height: gvdialogs.cellHeight - Theme.paddingSmall
                context: fwddialogpage.context
                peer: model.peer
                title: model.title
                onClicked: forwardRequested(model)
            }
        }
    }
}
