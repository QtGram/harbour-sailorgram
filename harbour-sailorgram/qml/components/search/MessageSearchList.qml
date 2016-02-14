import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../items/search"

Item
{
    property Context context
    property alias searchBox: searchbox
    property alias count: lvsearch.count

    signal messageClicked(var message)

    id: searchlist
    height: {
        var h = lvsearch.contentHeight;
        if(searchbox.visible)
            h += searchbox.height;
        return h;
    }

    SearchField
    {
        id: searchbox
        anchors { left: parent.left; right: parent.right }
        visible: context.showsearchfield
        height: visible ? Theme.itemSizeSmall : 0

        Behavior on height { NumberAnimation { duration: 150 } }

        onVisibleChanged: {
            if(visible)
                return;
            text = "";
        }
    }

    SilicaListView
    {
        id: lvsearch
        anchors { left: parent.left; top: searchbox.bottom; right: parent.right; bottom: parent.bottom }
        spacing: Theme.paddingMedium
        clip: true

        model: SearchModel {
            id: searchmodel
            telegram: context.telegram
            keyword: searchbox.text
        }

        delegate: MessageSearchItem {
            id: searchitem
            contentWidth: parent.width
            contentHeight: Theme.itemSizeSmall
            context: searchlist.context
            message: item
            onClicked: messageClicked(searchitem.message);
        }
    }
}

