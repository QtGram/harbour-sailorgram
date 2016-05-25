import QtQuick 2.0
import Sailfish.Silica 1.0
import QtLocation 5.0
import QtPositioning 5.0
import harbour.sailorgram.TelegramQml 2.0 as Telegram
import harbour.sailorgram.SailorGram 1.0
import "../../models"
import "../../items/sticker"

Dialog
{
    property Context context
    property var selectedSticker: null

    signal actionCompleted(int actiontype, var data)
    signal actionRequested(int actiontype)
    signal actionRejected()

    id: dlgselectorsticker
    allowedOrientations: defaultAllowedOrientations
    acceptDestinationAction: PageStackAction.Pop
    canAccept: selectedSticker !== null
    onAccepted: actionCompleted(-1, selectedSticker)

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        context.stickers.telegram = context.telegram;
    }

    DialogHeader
    {
        id: header
        acceptText: qsTr("Send")

        title: {
            var stickerset = context.stickers.stickerSetItem(context.stickers.currentStickerSet);

            if(stickerset)
                return stickerset.title;

            return "";
        }
    }

    SilicaGridView
    {
        readonly property int columns: (dlgselectorsticker.isPortrait ? 5 : 9)
        readonly property int spacing: Theme.paddingSmall
        readonly property real itemSize : (((width + spacing) / columns) - spacing)

        VerticalScrollDecorator { flickable: stickergrid }
        HorizontalScrollDecorator { flickable: stickergrid }

        id: stickergrid
        anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: lvstickersets.top }
        cellWidth: itemSize
        cellHeight: itemSize
        model: context.stickers
        clip: true

        delegate: StickerItem {
            id: stickeritem
            width: stickergrid.itemSize - stickergrid.spacing
            height: stickergrid.itemSize - stickergrid.spacing
            context: dlgselectorsticker.context
            stickerDocument: document
            highlighted: (selectedSticker === stickeritem.stickerDocument)

            onClicked: {
                selectedSticker = stickeritem.stickerDocument;
            }
        }

        ViewPlaceholder
        {
            enabled: context.stickers.count <= 0
            text: qsTr("Select a Sticker Set from below")
        }
    }

    SilicaListView
    {
        id: lvstickersets
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom; bottomMargin: Theme.paddingSmall }
        orientation: ListView.Horizontal
        height: Theme.itemSizeLarge
        model: context.stickers.stickerSets
        spacing: Theme.paddingSmall
        clip: true

        delegate: StickerSetImage {
            width: Theme.itemSizeMedium
            contentHeight: parent.height
            context: dlgselectorsticker.context
            stickersModel: context.stickers
            stickerSetId: modelData
            highlighted: (context.stickers.currentStickerSet === modelData)

            onClicked: {
                context.stickers.currentStickerSet = modelData;
            }
        }
    }
}
