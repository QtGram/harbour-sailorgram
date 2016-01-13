import QtQuick 2.0
import Sailfish.Silica 1.0
import QtLocation 5.0
import QtPositioning 5.0

Dialog
{
    property int selectedStickerId : -1

    signal actionCompleted(string action, var data)

    id: selectorstickerpage
    allowedOrientations: defaultAllowedOrientations
    canAccept:(selectedStickerId >= 0)

    onAccepted: {
        // TODO : @Dax89 send 'stickerId' to conversation
    }

    function getImagePathForSticker(stickerId) {
        // TODO : @Dax89 : handle using TelegramQML API to retrieve the file path for the sticker
        return dumbStickersModel.dumbImages [stickerId % dumbStickersModel.dumbImages.length]
    }

    function getImagePathForStickersPack(stickersSetName) {
        // TODO : @Dax89 : handle using TelegramQML API to retrieve the file path for the sticker pack
        return "image://theme/icon-m-other"
    }

    function getFullNameForStickersPack(stickersSetName) {
        // TODO : @Dax89 : handle using TelegramQML API to retrieve the full name for the sticker pack
        return "Full Name of the pack"
    }

    property QtObject stickersmodel : dumbStickersModel;  //  TODO : @Dax89 : this is for demo, bind the real StickersModel here

    ListModel
    {
        id: dumbStickersModel
        onCurrentStickerSetChanged: { refresh(); }
        Component.onCompleted: { refresh(); }

        property string currentStickerSet : "minds"

        readonly property var installedStickerSets : ["minds", "memes", "minions"]

        readonly property var dumbImages : [
            "image://theme/icon-m-car",
            "image://theme/icon-m-day",
            "image://theme/icon-m-edit",
            "image://theme/icon-m-home",
            "image://theme/icon-m-jolla",
            "image://theme/icon-m-night",
            "image://theme/icon-m-presence",
            "image://theme/icon-m-sailfish",
            "image://theme/icon-m-traffic",
            "image://theme/icon-m-train",
            "image://theme/icon-m-toy",
            "image://theme/icon-m-wizard",
        ]

        function refresh() {
            clear()
            for(var idx = 0; idx < 50; idx++) {
                append({
                            "stickerId"      : idx,
                            "document"       : 0,
                            "emoticon"       : ":-)",
                            "stickerSetName" : currentStickerSet,
                            "stickerSetId"   : installedStickerSets.indexOf(currentStickerSet),
                        })
            }
        }
    }

    DialogHeader
    {
        id: header
        title: getFullNameForStickersPack(stickersmodel.currentStickerSet)
    }

    SilicaFlickable
    {
        id: view
        clip: true
        contentHeight:(stickergrid.height + stickergrid.anchors.margins * 2)
        anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: layouttabs.top }

        ScrollDecorator { flickable: view }

        Grid
        {
            readonly property real itemSize :(((width + spacing) / columns) - spacing)

            id: stickergrid
            columns:(selectorstickerpage.isPortrait ? 5 : 9)
            spacing: Theme.paddingSmall
            anchors { top: parent.top; left: parent.left; right: parent.right; margins: Theme.paddingSmall }

            Repeater
            {
                model: stickersmodel

                delegate: MouseArea {
                    z: (selectedStickerId === model.stickerId ? 999999 : model.index)
                    scale: (selectedStickerId === model.stickerId ? 1.35 : 1.0)
                    width: stickergrid.itemSize
                    height: stickergrid.itemSize
                    onClicked: { selectedStickerId =(selectedStickerId !== model.stickerId ? model.stickerId : -1); }

                    Rectangle {
                        color: Theme.highlightColor
                        opacity: 0.85
                        visible:(selectedStickerId === model.stickerId)
                        anchors.fill: parent
                    }
                    Image {
                        source: getImagePathForSticker(model.stickerId)
                        fillMode: Image.PreserveAspectFit
                        verticalAlignment: Image.AlignVCenter
                        horizontalAlignment: Image.AlignHCenter
                        anchors {
                            fill: parent
                            margins:(Theme.paddingSmall / 2)
                        }
                    }
                }
            }
        }
    }

    Grid
    {
        readonly property real itemSize :(((width + spacing) / columns) - spacing)

        id: layouttabs
        columns: stickergrid.columns
        spacing: 1
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom; margins: 1 }

        Repeater
        {
            model: stickersmodel.installedStickerSets

            delegate: MouseArea {
                width: layouttabs.itemSize
                height: layouttabs.itemSize
                onClicked: { stickersmodel.currentStickerSet = modelData; }

                Rectangle
                {
                    color:(stickersmodel.currentStickerSet === modelData ? Theme.highlightColor : Theme.secondaryColor)
                    opacity: 0.35
                    anchors.fill: parent
                }

                Image
                {
                    source: getImagePathForStickersPack(modelData)
                    fillMode: Image.PreserveAspectFit
                    verticalAlignment: Image.AlignVCenter
                    horizontalAlignment: Image.AlignHCenter
                    anchors { fill: parent; margins: (Theme.paddingSmall / 2) }
                }
            }
        }
    }
}
