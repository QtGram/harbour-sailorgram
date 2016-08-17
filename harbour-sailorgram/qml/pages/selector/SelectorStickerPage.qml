import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.SailorGram 1.0
import harbour.sailorgram.TelegramQml 2.0 as Telegram
import "../../models"
import "../../items/sticker"

Dialog
{
    property Context context
    property string stickerTitle
    property var selectedSticker: null

    signal actionCompleted(int typingstatus, var data)
    signal actionRequested(int typingstatus)
    signal actionRejected()

    id: dlgselectorsticker
    allowedOrientations: defaultAllowedOrientations
    acceptDestinationAction: PageStackAction.Pop
    canAccept: selectedSticker !== null
    onAccepted: actionCompleted(-1, selectedSticker)

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;
    }

    DialogHeader
    {
        id: header
        acceptText: qsTr("Send")
        title: stickerTitle
    }

    SilicaGridView
    {
        readonly property int columns: (dlgselectorsticker.isPortrait ? 4 : 6)
        readonly property int spacing: Theme.paddingSmall
        readonly property real itemSize : (((width + spacing) / columns) - spacing)
        property alias documentItems: stickersmodel.documents

        VerticalScrollDecorator { flickable: stickergrid }
        HorizontalScrollDecorator { flickable: stickergrid }

        id: stickergrid
        anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: lvstickersets.top }
        cellWidth: itemSize
        cellHeight: itemSize
        clip: true

        model: Telegram.StickersModel {
            id: stickersmodel
            engine: context.engine
        }

        delegate: StickerItem {
            id: stickeritem
            width: stickergrid.itemSize - stickergrid.spacing
            height: stickergrid.itemSize - stickergrid.spacing
            context: dlgselectorsticker.context
            sticker: model.document
            highlighted: (selectedSticker === model.document)

            onClicked: {
                selectedSticker = stickeritem.sticker;
            }
        }


        ViewPlaceholder
        {
            enabled: (stickergrid.count <= 0)
            text: qsTr("Select a Sticker Set from below")
        }
    }

    SilicaListView
    {
        property int currentHash: 0

        id: lvstickersets
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom; bottomMargin: Theme.paddingSmall }
        orientation: ListView.Horizontal
        height: Theme.itemSizeLarge
        spacing: Theme.paddingSmall
        model: context.stickers
        clip: true

        delegate: StickerCategory {
            width: Theme.itemSizeMedium
            contentHeight: parent.height
            context: dlgselectorsticker.context
            document: model.count > 0 ? model.documentItems[0] : null
            highlighted: (lvstickersets.currentHash === model.hash)

            Component.onCompleted: {
                if((model.index !== 0) || (stickerTitle.length > 0))
                    return;

                stickerTitle = model.title;
                lvstickersets.currentHash = model.hash;
                stickergrid.documentItems = model.documentItems;
            }

            onClicked: {
                stickerTitle = model.title;
                lvstickersets.currentHash = model.hash;
                stickergrid.documentItems = model.documentItems;
            }
        }
    }
}
