import QtQuick 2.0
import Sailfish.Silica 1.0
import QtLocation 5.0
import QtPositioning 5.0
import harbour.sailorgram.TelegramQml 1.0
import harbour.sailorgram.SailorGram 1.0
import "../../models"
import "../../items/sticker"

Page
{
    property Context context

    signal actionCompleted(string action, var data)

    id: selectorstickerpage
    allowedOrientations: defaultAllowedOrientations

    RemorsePopup { id: remorsepopup }

    StickersModel
    {
        id: stickersmodel
        telegram: context.telegram
    }

    PageHeader
    {
        id: header

        title: {
            var stickerset = stickersmodel.stickerSetItem(stickersmodel.currentStickerSet);

            if(stickerset)
                return stickerset.title;

            return "";
        }
    }

    SilicaFlickable
    {
        id: view
        clip: true
        contentHeight: (stickergrid.height + stickergrid.anchors.margins * 2)
        anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: lvstickersets.top }

        SilicaGridView
        {
            readonly property int columns: (selectorstickerpage.isPortrait ? 5 : 9)
            readonly property int spacing: Theme.paddingSmall
            readonly property real itemSize : (((width + spacing) / columns) - spacing)

            VerticalScrollDecorator { flickable: stickergrid }
            HorizontalScrollDecorator { flickable: stickergrid }

            id: stickergrid
            anchors.fill: parent
            cellWidth: itemSize
            cellHeight: itemSize
            model: stickersmodel

            delegate: StickerItem {
                id: stickeritem
                width: stickergrid.itemSize - stickergrid.spacing
                height: stickergrid.itemSize - stickergrid.spacing
                context: selectorstickerpage.context
                stickerDocument: document

                onClicked: {
                    remorsepopup.execute(qsTr("Sending sticker"), function() {
                        actionCompleted("sticker", stickeritem.stickerPath);
                    });
                }
            }
        }
    }

    SilicaListView
    {
        id: lvstickersets
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        orientation: ListView.Horizontal
        height: Theme.itemSizeMedium + Theme.paddingSmall
        model: stickersmodel.stickerSets
        currentIndex: -1
        spacing: Theme.paddingSmall

        delegate: StickerSetImage {
            width: Theme.itemSizeMedium
            height: parent.height
            context: selectorstickerpage.context
            stickersModel: stickersmodel
            stickerSetId: modelData
            isSelected: (lvstickersets.currentIndex === index)

            onClicked: {
                stickersmodel.currentStickerSet = modelData;
            }
        }
    }
}
