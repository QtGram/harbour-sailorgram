import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../js/TelegramHelper.js" as TelegramHelper

SilicaListView
{
    property alias folderModel: filepicker.model

    signal fileSelected(string filepath)
    signal folderSelected(string folderpath)
    signal previewRequested(string filepath)

    id: filepicker
    quickScroll: true

    delegate:  ListItem {
        id: listitem
        contentHeight: Theme.itemSizeSmall
        contentWidth: parent.width

        onClicked: {
            if(isfile)
                remorseAction(qsTr("Picking file"), function () { fileSelected(filepath); });
            if(isfolder)
                folderSelected(filepath);
        }

        Image
        {
            id: imgicon
            source: fileicon
            anchors { left: parent.left; verticalCenter: parent.verticalCenter }
            asynchronous: true
            fillMode: Image.PreserveAspectFit
        }

        Label
        {
            id: lblfilename
            anchors { left: imgicon.right; right: btnpreview.left; top: parent.top; leftMargin: Theme.paddingMedium; }
            verticalAlignment: Text.AlignTop
            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            width: parent.width
            text: filename
        }

        Row
        {
            anchors { left: imgicon.right; right: btnpreview.right; top: lblfilename.bottom; leftMargin: Theme.paddingMedium }
            height: parent.height - lblfilename.contentHeight
            spacing: Theme.paddingSmall

            Label
            {
                id: lblcreated
                wrapMode: Text.NoWrap
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: Theme.fontSizeTiny
                color: Theme.secondaryHighlightColor
                text: qsTr("Created:")
            }

            Label
            {
                id: lblcreationdate
                width: parent.width - lblcreated.contentWidth
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: Theme.fontSizeTiny
                text: TelegramHelper.printableDate(filedate, true)
            }
        }

        IconButton
        {
            id: btnpreview
            visible: isimage
            anchors { right: parent.right; verticalCenter: parent.verticalCenter; rightMargin: Theme.paddingSmall }
            icon.source: "qrc:///res/preview.png"
            onClicked: previewRequested(filepath)
        }
    }
}
