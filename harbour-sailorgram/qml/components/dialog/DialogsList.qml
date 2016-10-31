import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../model"
import "../../items"

SilicaListView
{
    property Context context
    property var lastDialog: null

    function openDialog(dialog) {
        if(dialog !== dialogslist.lastDialog) {
            dialogslist.lastDialog = dialog;
            pageStack.pushAttached(Qt.resolvedUrl("../../pages/dialog/DialogPage.qml"), { context: dialogslist.context, dialog: dialog });
        }

        pageStack.navigateForward();
    }

    id: dialogslist
    spacing: Theme.paddingSmall
    clip: true

    model: DialogsModel {
        telegram: context.telegram
    }

    delegate: DialogModelItem {
        width: parent.width
        onClicked: openDialog(model.item)
    }
}
