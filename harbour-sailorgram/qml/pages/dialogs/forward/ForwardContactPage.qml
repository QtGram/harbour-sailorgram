import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../../models"
import "../../../components/search"
import "../../../items/message/forward"

Page
{
    property Context context

    signal forwardRequested(var dialogmodelitem)

    id: fwdcontactpage

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        context.contacts.engine = context.engine;
    }

    SilicaFlickable
    {
        anchors.fill: parent

        PageHeader
        {
            id: pageheader
            title: qsTr("Forward to contact")
        }

        SilicaGridView
        {
            id: gvcontacts
            anchors { left: parent.left; top: pageheader.bottom; right: parent.right; bottom: parent.bottom; leftMargin: Theme.paddingMedium }
            cellWidth: Theme.iconSizeExtraLarge
            cellHeight: Theme.iconSizeExtraLarge
            model: context.contacts
            clip: true

            delegate: ForwardContactItem { // NOTE: FCI Deprecated
                width: gvcontacts.cellWidth - Theme.paddingSmall
                height: gvcontacts.cellHeight - Theme.paddingSmall
                context: fwdcontactpage.context
                peer: model.peer
                title: model.title
                onClicked: forwardRequested(model)
            }
        }
    }
}
