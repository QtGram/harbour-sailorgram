import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../model"
import "../../items"

Page
{
    property Context context
    property var fromDialog

    signal forwardRequested(var todialog)

    id: forwardpage
    allowedOrientations: defaultAllowedOrientations

    SilicaListView
    {
        id: lvforward
        anchors.fill: parent
        spacing: Theme.paddingLarge

        header: PageHeader {
            title: qsTr("Forward message(s)")
        }

        model: ForwardDialogsModel {
            id: forwarddialogsmodel
            telegram: context.telegram
            fromDialog: forwardpage.fromDialog
        }

        delegate: ForwardDialogModelItem {
            width: parent.width
            onClicked: forwardRequested(model.item)
        }
    }
}
