import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.sailorgram.ImagesModel 1.0
import "../../models"

Dialog
{
    property alias directory: imagesmodel.rootDir
    property alias sortRole: imagesmodel.sortRole
    property alias sortOrder: imagesmodel.sortOrder
    property string selectedFile
    property Page rootPage
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
    canAccept: selectedFile.length > 0
    onAccepted: actionCompleted("image", selectedFile)

    ImagesModel {
        id: imagesmodel
        sortRole: ImagesModel.DateRole
        sortOrder: Qt.DescendingOrder
        recursive: false
        directoriesFirst: true
    }

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: height - pulldownmenu.spacing

        PullDownMenu
        {
            id: pulldownmenu

            MenuItem
            {
                text: qsTr("Android storage")
                visible: context.sailorgram.androidStorage.length > 0
                onClicked: {
                    if (!!rootPage) {
                        rootPage.directory = context.sailorgram.androidStorage;
                        rootPage.sortRole = ImagesModel.NameRole;
                        rootPage.sortOrder = Qt.AscendingOrder;
                        pageStack.pop(rootPage);
                    } else {
                        directory = context.sailorgram.androidStorage;
                        sortRole = ImagesModel.NameRole;
                        sortOrder = Qt.AscendingOrder;
                    }
                }
            }

            MenuItem {
                text: qsTr("SD Card")
                visible: context.sailorgram.sdcardFolder.length > 0
                onClicked: {
                    if (!!rootPage) {
                        rootPage.directory = context.sailorgram.sdcardFolder;
                        rootPage.sortRole = ImagesModel.NameRole;
                        rootPage.sortOrder = Qt.AscendingOrder;
                        pageStack.pop(rootPage);
                    } else {
                        directory = context.sailorgram.sdcardFolder;
                        sortRole = ImagesModel.NameRole;
                        sortOrder = Qt.AscendingOrder;
                    }
                }
            }

            MenuItem {
                text: qsTr("Home")
                onClicked: {
                    if (!!rootPage) {
                        rootPage.directory = context.sailorgram.homeFolder;
                        rootPage.sortRole = ImagesModel.NameRole;
                        rootPage.sortOrder = Qt.AscendingOrder;
                        pageStack.pop(rootPage);
                    } else {
                        directory = context.sailorgram.homeFolder;
                        sortRole = ImagesModel.NameRole;
                        sortOrder = Qt.AscendingOrder;
                    }
                }
            }

            MenuItem {
                text: qsTr("Sort ascending")
                visible: sortOrder !== Qt.AscendingOrder
                onClicked: sortOrder = Qt.AscendingOrder
            }

            MenuItem {
                text: qsTr("Sort descending")
                visible: sortOrder !== Qt.DescendingOrder
                onClicked: sortOrder = Qt.DescendingOrder
            }

            MenuItem {
                text: qsTr("Sort by name")
                visible: sortRole !== ImagesModel.NameRole
                onClicked: sortRole = ImagesModel.NameRole
            }

            MenuItem {
                text: qsTr("Sort by date")
                visible: sortRole !== ImagesModel.DateRole
                onClicked: sortRole = ImagesModel.DateRole
            }
        }

        DialogHeader
        {
            id: header
            acceptText: qsTr("Send")
            title: imagesmodel.rootDir.split('/').pop();
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
                property bool isSelected: (selectedFile === model.url)

                width: cellSize
                height: cellSize
                highlighted: isSelected

                onClicked: {
                    if (model.isDir) {
                        var nextPage = pageStack.push(Qt.resolvedUrl("SelectorImagesPage.qml"),
                                                      { "directory": model.path,
                                                        "context": context,
                                                        "sortRole": sortRole,
                                                        "sortOrder": sortOrder,
                                                        "rootPage": !!rootPage ? rootPage : selectorimagespage } );
                        nextPage.actionCompleted.connect(selectorimagespage.actionCompleted);
                    }
                    else if (view.currentIndex !== index) {
                        view.currentIndex = index;
                        selectedFile = model.url.toString();
                    }
                }

                Image {
                    id: thumbnail
                    width: view.cellWidth
                    height: model.isDir ? view.cellHeight - labelloader.item.height : view.cellHeight
                    fillMode: model.isDir ? Image.PreserveAspectFit : Image.PreserveAspectCrop
                    asynchronous: true
                    antialiasing: true
                    sourceSize.width: maxCellSize
                    sourceSize.height: maxCellSize
                    rotation: model.isDir ? 0 : 360 - model.orientation
                    source: model.isDir ? "image://theme/icon-m-folder" : "image://thumbnail/" + model.path;
                }

                Loader {
                    id: labelloader
                    active: model.isDir
                    anchors.bottom: parent.bottom
                    width: view.cellWidth
                    height: active && !!item ? Math.min(item.contentHeight, view.cellHeight * 2 / 3) : 0

                    sourceComponent: Label {
                        text: model.name
                        horizontalAlignment: Qt.AlignHCenter
                        wrapMode: Text.Wrap
                        clip: true
                    }
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
}
