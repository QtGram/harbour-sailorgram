import QtQuick 2.1
import Sailfish.Silica 1.0
import "../custom"

BackgroundRectangle
{
    readonly property bool expanded: height > 0
    property real contentHeight: Theme.itemSizeSmall

    id: dialogmediapanel
    height: 0
    clip: true

    Behavior on height {
        NumberAnimation {
            duration: 250; easing.type: Easing.OutQuad
        }
    }

    function hide() {
        height = 0;
    }

    function show() {
        height = contentHeight;
    }

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
            icon.source: "image://theme/icon-m-document"

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
