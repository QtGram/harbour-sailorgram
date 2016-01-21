import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.sailorgram.ImagesModel 1.0
import "../../models"

Page
{
    property Context context
    property int numCellColumnsPortrait: {
        switch (Screen.sizeCategory) {
            case Screen.Small: return 2;
            case Screen.Medium: return 3;
            case Screen.Large: return 4;
            case Screen.ExtraLarge: return 5;
            default: return 3;
        }
    }
    property int numCellColumnsLandscape: {
        switch (Screen.sizeCategory) {
            case Screen.Small: return 4;
            case Screen.Medium: return 5;
            case Screen.Large: return 6;
            case Screen.ExtraLarge: return 7;
            default: return 5;
        }
    }
    readonly property real cellSize: isPortrait ? width / numCellColumnsPortrait : width / numCellColumnsLandscape
    readonly property real maxCellSize: Math.max(Screen.height / numCellColumnsLandscape, Screen.width / numCellColumnsPortrait)

    signal actionCompleted(string action, var data)

    id: selectorimagespage
    allowedOrientations: defaultAllowedOrientations

    ImagesModel {
        id: imagesmodel
        sortRole: ImagesModel.DateRole
        sortOrder: Qt.DescendingOrder
    }

    RemorsePopup {
        id: remorsepopup
        onCanceled: view.currentIndex = -1
    }

    PageHeader
    {
        id: header
        title: qsTr("Images")
    }

    SilicaGridView
    {
        id: view
        clip: true
        anchors { left: parent.left; top: header.bottom; right: parent.right; bottom: parent.bottom }
        quickScroll: true
        cellWidth: cellSize
        cellHeight: cellSize
        model: imagesmodel

        delegate: BackgroundItem {
            width: cellSize
            height: cellSize
            onClicked: {
                if (view.currentIndex !== index) {
                    remorsepopup.cancel();
                    view.currentIndex = index;
                    remorsepopup.execute(qsTr("Selecting image"), function () { actionCompleted("image", model.url) });
                }
            }

            Image {
                id: thumbnail
                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                antialiasing: true
                sourceSize.width: maxCellSize
                sourceSize.height: maxCellSize
                rotation: 360 - model.orientation
                source: "image://thumbnail/" + model.path;
            }

            Loader {
                active: (thumbnail.status === Image.Loading)
                anchors.centerIn: parent
                width: view.cellWidth / 2
                height: view.cellHeight / 2

                sourceComponent: BusyIndicator {
                    running: true
                }
            }

            Loader {
                active: index === view.currentIndex
                anchors.fill: parent

                sourceComponent: Rectangle {
                    color: Theme.highlightColor
                    opacity: 0.4
                }
            }
        }
    }
}
