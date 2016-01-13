import QtQuick 2.0
import Sailfish.Silica 1.0
import Qt.labs.folderlistmodel 2.1
import harbour.sailorgram.SailorGram 1.0
import harbour.sailorgram.Selector 1.0
import "../../models"

Page
{
    property Context context
    readonly property string rootPathLimit : "file://" + context.sailorgram.homeFolder

    signal actionCompleted(string action, var data)

    id: selectorimagespage
    allowedOrientations: defaultAllowedOrientations

    FolderListModel {
        id: folderlistmodel
        folder: context.sailorgram.picturesFolder
        showFiles: true
        showHidden: false
        showDirsFirst: true
        showDotAndDotDot: false
        showOnlyReadable: true
        nameFilters: ["*.jpg", "*.jpeg", "*.png", "*.gif", "*.bmp", "*.webp"]
    }

    RemorsePopup { id: remorsepopup }

    PageHeader
    {
        id: header
        title: folderlistmodel.folder.toString().replace("file:///", "/")
    }

    SilicaFlickable
    {
        id: view
        clip: true
        anchors { left: parent.left; top: header.bottom; right: parent.right; bottom: parent.bottom }
        contentHeight: (layoutflow.height + layoutflow.anchors.margins * 2)

        VerticalScrollDecorator { flickable: view }
        HorizontalScrollDecorator { flickable: view }

        Flow
        {
            readonly property real cellSize : (((width + spacing) / (selectorimagespage.isPortrait ? 4 : 7)) - spacing)

            id: layoutflow
            spacing: Theme.paddingSmall
            anchors { top: parent.top; left: parent.left; right: parent.right; margins: spacing }

            Button {
                width: layoutflow.width
                height: Theme.itemSizeSmall
                visible: (folderlistmodel.folder.toString() !== rootPathLimit)
                text: qsTr ("Back")

                onClicked: {
                    folderlistmodel.folder = folderlistmodel.parentFolder;
                }
            }

            Repeater
            {
                model: folderlistmodel

                delegate: BackgroundItem {
                    readonly property bool isFolder : (model.fileIsDir || false)
                    readonly property string imageUrl : (model.fileURL ? model.fileURL.toString () : "")

                    id: clicker
                    width: (isFolder ? layoutflow.width : layoutflow.cellSize)
                    height: (isFolder ? Theme.itemSizeSmall : layoutflow.cellSize)
                    opacity: (enabled ? 1.0 : 0.35)

                    onClicked: {
                        if (isFolder)
                            folderlistmodel.folder = imageUrl;
                        else
                            remorsepopup.execute(qsTr("Selecting image"), function () { actionCompleted("image", imageUrl) });
                    }

                    Row {
                        visible: isFolder
                        spacing: Theme.paddingMedium
                        height: Theme.itemSizeSmall
                        width: parent.width

                        Image {
                            source: "image://theme/icon-m-folder"
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Label {
                            text: (model.fileName || "")
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    Image {
                        id: thumbnail
                        cache: false
                        visible: !isFolder
                        fillMode: Image.PreserveAspectCrop
                        asynchronous: true
                        antialiasing: true
                        sourceSize.width: 150
                        sourceSize.height: 150
                        anchors.fill: parent

                        ImageThumbnailer on source {
                            id: thumbnailer
                            imageUrl: (thumbnail.visible ? (model.fileURL || "") : "")
                        }
                    }

                    BusyIndicator {
                        size: BusyIndicatorSize.Medium
                        visible: running
                        running: (thumbnailer.status === ImageThumbnailer.Processing)
                        anchors.centerIn: parent
                    }
                }
            }
        }
    }
}
