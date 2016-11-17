import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../model"
import "../../items"

SilicaListView
{
    property Context context

    property ContactsModel contactsModel: ContactsModel {
        id: contactsmodel
        telegram: context.telegram
    }

    id: contactslist
    spacing: Theme.paddingSmall
    model: contactsmodel

    delegate: ContactModelItem {
        width: parent.width
        onClicked: contactsmodel.createDialog(model.item)
    }
}
