import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../model"
import "../../items"

SilicaListView
{
    property Context context

    id: contactslist
    spacing: Theme.paddingSmall

    model: ContactsModel {
        telegram: context.telegram
    }

    delegate: ContactModelItem {
        width: parent.width
    }
}
