import QtQuick 2.1
import Sailfish.Silica 1.0

DockedPanel
{
    id: dialogmediapanel
    dock: Dock.Bottom
    height: Theme.itemSizeSmall

    signal shareImage()
    signal shareFile()
    signal shareLocation()

    Row
    {
        anchors.centerIn: parent
        spacing: Theme.paddingLarge

        IconButton
        {
            id: btnshareimage
            icon.source: "image://theme/icon-m-image"

            onClicked: {
                dialogmediapanel.hide();
                shareImage();
            }
        }

        IconButton
        {
            id: btnsharefile
            icon.source: "image://theme/icon-m-file-document"

            onClicked: {
                dialogmediapanel.hide();
                shareFile();
            }
        }

        IconButton
        {
            id: btnsharelocation
            icon.source: "image://theme/icon-m-location"

            onClicked: {
                dialogmediapanel.hide();
                shareLocation();
            }
        }
    }
}
