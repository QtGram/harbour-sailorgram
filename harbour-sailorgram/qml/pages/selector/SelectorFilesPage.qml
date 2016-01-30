import QtQuick 2.0
import Sailfish.Silica 1.0
import Qt.labs.folderlistmodel 2.1
import harbour.sailorgram.SailorGram 1.0
import "../../models"

Dialog
{
    property Context context
    property string selectedFile
    readonly property string rootPathLimit : "file:///"

    signal actionCompleted(string action, var data)

    id: selectorfilespage
    allowedOrientations: defaultAllowedOrientations
    canAccept: selectedFile.length > 0
    onAccepted: actionCompleted("file", selectedFile)

    FolderListModel
    {
        id: folderlistmodel
        folder: context.sailorgram.homeFolder
        showFiles: true
        showHidden: false
        showDirsFirst: true
        showOnlyReadable: true
        showDotAndDotDot: false
    }

    SilicaFlickable
    {
        anchors.fill: parent

        PullDownMenu
        {
            MenuItem
            {
                text: qsTr("Android storage")
                visible: context.sailorgram.androidStorage.length > 0

                onClicked: {
                    folderlistmodel.folder = context.sailorgram.androidStorage;
                }
            }

            MenuItem {
                text: qsTr("SD Card")
                visible: context.sailorgram.sdcardFolder.length > 0

                onClicked: {
                    folderlistmodel.folder = context.sailorgram.sdcardFolder;
                }
            }

            MenuItem {
                text: qsTr("Home")

                onClicked: {
                    folderlistmodel.folder = context.sailorgram.homeFolder;
                }
            }
        }

        DialogHeader
        {
            id: header
            acceptText: qsTr("Send")
            title: folderlistmodel.folder.toString().replace("file:///", "/")
        }

        Button
        {
            id: btnparent
            text: qsTr("Back")
            visible: (folderlistmodel.folder.toString () !== rootPathLimit)
            anchors { left: parent.left; top: header.bottom; right: parent.right; margins: Theme.paddingMedium }

            onClicked: {
                folderlistmodel.folder = folderlistmodel.parentFolder;
            }
        }

        SilicaListView
        {
            id: lvfiles
            clip: true
            model: folderlistmodel
            anchors { top: btnparent.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }

            delegate: ListItem {
                property string fileUrl: model.fileURL.toString()
                property bool isSelected: (selectedFile === fileUrl)

                contentHeight: Theme.itemSizeSmall
                anchors { left: parent.left; right: parent.right }
                highlighted: isSelected

                onClicked: {
                    if (model.fileIsDir)
                        folderlistmodel.folder = model.fileURL;
                    else
                        selectedFile = fileUrl;
                }

                Image {
                    id: imgfilefolder
                    source: (model.fileIsDir ? "image://theme/icon-m-folder" : "image://theme/icon-m-other")
                    anchors { left: parent.left; margins: Theme.paddingMedium; verticalCenter: parent.verticalCenter }
                }

                Label {
                    text: (model.fileName || "")
                    elide: Text.ElideMiddle
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    maximumLineCount: 2
                    font.family: Theme.fontFamilyHeading
                    font.bold: isSelected
                    color: isSelected ? Theme.highlightColor : Theme.primaryColor
                    anchors { left: imgfilefolder.right; right: parent.right; margins: Theme.paddingMedium; verticalCenter: parent.verticalCenter }
                }
            }

            VerticalScrollDecorator { flickable: lvfiles }
        }
    }
}
