import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../model"
import "../../items"

SilicaListView
{
    property Context context

    id: dialogslist
    spacing: Theme.paddingSmall
    clip: true

    model: DialogsModel {
        telegram: context.telegram
    }

    delegate: DialogModelItem {
        width: parent.width

        onClicked: {
            pageStack.push(Qt.resolvedUrl("../../pages/dialog/DialogPage.qml"), { context: dialogslist.context,
                                                                                  dialog: model.item });
        }
    }
}
