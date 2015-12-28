import QtQuick 2.1
import Sailfish.Silica 1.0

SilicaListView
{
    property alias folderModel: filepicker.model

    signal fileSelected(string filepath)
    signal folderSelected(string folderpath)
    signal previewRequested(string filepath)

    id: filepicker

    delegate:  ListItem {
        id: listitem
        contentHeight: Theme.itemSizeSmall
        contentWidth: parent.width

        Image
        {
            id: imgicon
            source: fileicon
            anchors { left: parent.left; verticalCenter: lblfilename.verticalCenter }
            asynchronous: true
            width: parent.height
            height: parent.height
            fillMode: Image.PreserveAspectFit
        }

        Label
        {
            id: lblfilename
            anchors { left: imgicon.right; right: btnpreview.left; verticalCenter: parent.verticalCenter; leftMargin: Theme.paddingMedium; }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            width: parent.width
            text: filename
        }

        IconButton
        {
            id: btnpreview
            visible: isimage
            anchors { right: parent.right; verticalCenter: parent.verticalCenter; rightMargin: Theme.paddingSmall }
            icon.source: "qrc:///res/preview.png"
            onClicked: previewRequested(filepath)
        }

        onClicked: {
            if(isfile)
                fileSelected(filepath);
            else if(isfolder)
                folderSelected(filepath);
        }
    }
}
