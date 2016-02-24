import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.sailorgram.FilesModel 1.0
import "../../models"

Dialog
{
    property alias folder: filesmodel.folder
    property alias sortRole: filesmodel.sortRole
    property alias sortOrder: filesmodel.sortOrder
    property var selectedFiles: []
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
    acceptDestinationAction: PageStackAction.Pop
    canAccept: selectedFiles.length > 0
    onAccepted: selectedFiles.forEach(function (element) { actionCompleted("image", element); })
    onRejected: if (!!rootPage) { rootPage.selectedFiles = selectedFiles; }

    FilesModel {
        id: filesmodel
        folder: "StandardImagesFolder"
        sortRole: FilesModel.DateRole
        filter: FilesModel.ImageFilter
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
                        rootPage.folder = context.sailorgram.androidStorage;
                        rootPage.sortRole = FilesModel.NameRole;
                        rootPage.sortOrder = Qt.AscendingOrder;
                        pageStack.pop(rootPage);
                    } else {
                        folder = context.sailorgram.androidStorage;
                        sortRole = FilesModel.NameRole;
                        sortOrder = Qt.AscendingOrder;
                    }
                }
            }

            MenuItem {
                text: qsTr("SD Card")
                visible: context.sailorgram.sdcardFolder.length > 0
                onClicked: {
                    if (!!rootPage) {
                        rootPage.folder = context.sailorgram.sdcardFolder;
                        rootPage.sortRole = FilesModel.NameRole;
                        rootPage.sortOrder = Qt.AscendingOrder;
                        pageStack.pop(rootPage);
                    } else {
                        folder = context.sailorgram.sdcardFolder;
                        sortRole = FilesModel.NameRole;
                        sortOrder = Qt.AscendingOrder;
                    }
                }
            }

            MenuItem {
                text: qsTr("Home")
                onClicked: {
                    if (!!rootPage) {
                        rootPage.folder = context.sailorgram.homeFolder;
                        rootPage.sortRole = FilesModel.NameRole;
                        rootPage.sortOrder = Qt.AscendingOrder;
                        pageStack.pop(rootPage);
                    } else {
                        folder = context.sailorgram.homeFolder;
                        sortRole = FilesModel.NameRole;
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
                visible: sortRole !== FilesModel.NameRole
                onClicked: sortRole = FilesModel.NameRole
            }

            MenuItem {
                text: qsTr("Sort by date")
                visible: sortRole !== FilesModel.DateRole
                onClicked: sortRole = FilesModel.DateRole
            }
        }

        DialogHeader
        {
            id: header
            acceptText: selectedFiles.length ? qsTr("Send %n image(s)", "", selectedFiles.length) :
                                               qsTr("Select images")
            cancelText: !!rootPage ? qsTr("Back") : qsTr("Cancel")
            title: filesmodel.folder.split('/').pop();
        }

        SilicaGridView
        {
            id: view
            clip: true
            anchors { left: parent.left; top: header.bottom; right: parent.right; bottom: parent.bottom }
            quickScroll: true
            cellWidth: cellSize
            cellHeight: cellSize
            model: filesmodel

            delegate: BackgroundItem {
                id: delegate

                property bool isSelected: (selectedFiles.indexOf(model.url) > -1)

                width: cellSize
                height: cellSize
                highlighted: isSelected

                onClicked: {
                    if (model.isDir) {
                        var nextPage = pageStack.push(Qt.resolvedUrl("SelectorImagesPage.qml"),
                                                      { "folder": model.path,
                                                        "context": context,
                                                        "sortRole": sortRole,
                                                        "sortOrder": sortOrder,
                                                        "selectedFiles": selectedFiles,
                                                        "acceptDestination": acceptDestination,
                                                        "rootPage": !!rootPage ? rootPage : selectorimagespage } );
                        nextPage.actionCompleted.connect(actionCompleted);
                    }
                    else {
                        //selectedFiles needs to be reassigned every time it is manipulated because it doesn't emit signals otherwise
                        if (isSelected) {
                            selectedFiles = selectedFiles.filter(function (element) { return element !== model.url; });
                        } else {
                            selectedFiles = selectedFiles.concat([model.url]);
                        }
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
                        verticalAlignment: Qt.AlignTop
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
                    active: delegate.isSelected
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
