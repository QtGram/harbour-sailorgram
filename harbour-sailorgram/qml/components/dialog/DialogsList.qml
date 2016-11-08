import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../model"
import "../../items"

SilicaListView
{
    property Context context
    property var lastDialog: null

    function openDialog(dialog, focus) {
        if(dialog !== dialogslist.lastDialog) {
            dialogslist.lastDialog = dialog;
            pageStack.pushAttached(Qt.resolvedUrl("../../pages/dialog/DialogPage.qml"), { context: dialogslist.context, dialog: dialog, firstLoad: true });
        }

        pageStack.navigateForward((focus === true) ? PageStackAction.Immediate : PageStackAction.Animated);

         if((focus === true) && (Qt.application.state !== Qt.ApplicationActive))
             mainwindow.activate();
    }

    Connections
    {
        target: context.sailorgram

         onOpenDialogRequested: {
            if(pageStack.depth > 1)
                pageStack.pop(context.mainPage, PageStackAction.Immediate);

            var dialog = context.dialogs.getDialog(dialogid);

            if(!dialog)
                return;

            openDialog(dialog, true);
         }
    }

    id: dialogslist
    spacing: Theme.paddingSmall
    model: context.dialogs
    clip: true

    delegate: DialogModelItem {
        width: parent.width
        onClicked: openDialog(model.item)
    }
}
