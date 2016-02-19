import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../../models"
import "../../../components/search"
import "../../../items/message/forward"

Page
{
    property Context context

    signal forwardRequested(int peerid, bool iscontact)

    id: fwdcontactpage

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        context.contacts.telegram = context.telegram;
        gvcontacts.model = context.contacts;
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
            clip: true

            delegate: ForwardContactItem {
                width: gvcontacts.cellWidth - Theme.paddingSmall
                height: gvcontacts.cellHeight - Theme.paddingSmall
                context: fwdcontactpage.context
                user: model.user
                onClicked: forwardRequested(model.user.id, true)
            }
        }
    }
}
