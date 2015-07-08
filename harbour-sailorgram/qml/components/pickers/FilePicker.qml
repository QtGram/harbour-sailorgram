import QtQuick 2.1
import Sailfish.Silica 1.0

SilicaListView
{
    property bool showThumbnails: false
    property bool multiSelect: false
    property alias folderModel: filepicker.model

    signal fileSelected(string filepath)
    signal folderSelected(string folderpath)

    id: filepicker

    delegate: ListItem {
        id: listitem
        contentHeight: Theme.itemSizeSmall
        contentWidth: parent.width

        Image
        {
            id: imgicon
            source: (showThumbnails && isimage) ? filepath : fileicon
            anchors { left: parent.left; verticalCenter: lblfilename.verticalCenter }
            width: parent.height
            height: parent.height
            fillMode: Image.PreserveAspectFit
            asynchronous: true
        }

        Label
        {
            id: lblfilename
            anchors { left: imgicon.right; right: swselect.left; verticalCenter: parent.verticalCenter; leftMargin: Theme.paddingMedium; }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            width: parent.width
            text: filename
        }

        Switch
        {
            id: swselect
            visible: isfile && multiSelect
            anchors { right: parent.right; verticalCenter: lblfilename.verticalCenter }
        }

        onClicked: {
            if(multiSelect)
            {
                swselect.checked = !swselect.checked
                return;
            }

            if(isfile)
                fileSelected(filepath);
            else if(isfolder)
                folderSelected(filepath);
        }
    }
}
