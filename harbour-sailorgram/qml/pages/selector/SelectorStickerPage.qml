import QtQuick 2.0
import Sailfish.Silica 1.0
import QtLocation 5.0
import QtPositioning 5.0
import harbour.sailorgram.TelegramQml 1.0 as TelegramQml
import harbour.sailorgram.SailorGram 1.0
import "../../models"
import "../../items/sticker"

Dialog
{
    property Context context
    property var selectedSticker: null

    signal actionCompleted(string action, var data)

    id: selectorstickerpage
    allowedOrientations: defaultAllowedOrientations
    canAccept: selectedSticker !== null
    onAccepted: actionCompleted("sticker", selectedSticker)

    TelegramQml.StickersModel
    {
        id: stickersmodel
        telegram: context.telegram
    }

    DialogHeader
    {
        id: header
        acceptText: qsTr("Send")

        title: {
            var stickerset = stickersmodel.stickerSetItem(stickersmodel.currentStickerSet);

            if(stickerset)
                return stickerset.title;

            return "";
        }
    }

    SilicaGridView
    {
        readonly property int columns: (selectorstickerpage.isPortrait ? 5 : 9)
        readonly property int spacing: Theme.paddingSmall
        readonly property real itemSize : (((width + spacing) / columns) - spacing)

        VerticalScrollDecorator { flickable: stickergrid }
        HorizontalScrollDecorator { flickable: stickergrid }

        id: stickergrid
        anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: lvstickersets.top }
        cellWidth: itemSize
        cellHeight: itemSize
        model: stickersmodel
        clip: true

        delegate: StickerItem {
            property bool isSelected: selectedSticker === stickeritem.stickerPath.toString()

            id: stickeritem
            width: stickergrid.itemSize - stickergrid.spacing
            height: stickergrid.itemSize - stickergrid.spacing
            context: selectorstickerpage.context
            stickerDocument: document
            highlighted: isSelected

            onClicked: {
                selectedSticker = stickeritem.stickerDocument;
            }
        }
    }

    SilicaListView
    {
        id: lvstickersets
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom; bottomMargin: Theme.paddingSmall }
        orientation: ListView.Horizontal
        height: Theme.itemSizeLarge
        model: stickersmodel.stickerSets
        currentIndex: 0
        spacing: Theme.paddingSmall
        clip: true

        delegate: StickerSetImage {
            width: Theme.itemSizeMedium
            contentHeight: parent.height
            highlighted: (lvstickersets.currentIndex === index)
            context: selectorstickerpage.context
            stickersModel: stickersmodel
            stickerSetId: modelData

            onClicked: {
                stickersmodel.currentStickerSet = modelData;
            }
        }
    }
}
