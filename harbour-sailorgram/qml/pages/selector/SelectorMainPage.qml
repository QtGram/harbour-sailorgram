import QtQuick 2.0
import Sailfish.Silica 1.0
import "../../models"

Page
{
    property Context context

    readonly property string fileAction: "file"
    readonly property string imageAction: "image"
    readonly property string locationAction: "location"
    readonly property string photoCaptureAction: "photocapture"
    readonly property string soundRecordAction: "soundrecord"
    readonly property string stickerAction: "sticker"

    signal actionCompleted(string action, var data)

    id: selectormainpage
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        context.stickers.telegram = context.telegram; // Preload stickers
    }

    PageHeader { id: header; title: qsTr ("Send file") }

    Item
    {
        anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }

        Grid
        {
            readonly  property real cellWidth  : (((width + spacing) / columns) - spacing)
            readonly  property real cellHeight : (((height + spacing) / rows) - spacing)

            id: grid
            rows: (selectormainpage.isPortrait ? 3 : 2)
            columns: (selectormainpage.isPortrait ? 2 : 3)
            spacing: Theme.paddingMedium
            anchors { fill: parent; margins: spacing }

            Repeater
            {
                model: [
                    { "label" : qsTr ("Image from gallery"),   "icon" : "image",      "page" : Qt.resolvedUrl("SelectorImagesPage.qml") },
                    { "label" : qsTr ("Capture photo"),        "icon" : "imaging",    "page" : Qt.resolvedUrl("SelectorPhotoCapturePage.qml") },
                    { "label" : qsTr ("Record sound"),         "icon" : "mic",        "page" : Qt.resolvedUrl("SelectorSoundRecordPage.qml") },
                    { "label" : qsTr ("My location"),          "icon" : "location",   "page" : Qt.resolvedUrl("SelectorLocationPage.qml") },
                    { "label" : qsTr ("Sticker"),              "icon" : "other",      "page" : Qt.resolvedUrl("SelectorStickerPage.qml") },
                    { "label" : qsTr ("Browse files"),         "icon" : "folder",     "page" : Qt.resolvedUrl("SelectorFilesPage.qml") },
                ]

                delegate: MouseArea {
                    readonly property color dynamicColor : (pressed ? Theme.highlightColor : Theme.primaryColor)

                    id: mousearea
                    width: grid.cellWidth
                    height: grid.cellHeight

                    onClicked: {
                        var page = pageStack.push(modelData ["page"], { "context": selectormainpage.context });

                        page.actionCompleted.connect(function(action, data) {
                            selectormainpage.actionCompleted(action, data);
                        });
                    }

                    Rectangle {
                        color: mousearea.dynamicColor
                        opacity: 0.15
                        anchors.fill: parent
                    }

                    Column {
                        spacing: Theme.paddingMedium
                        anchors { left: parent.left; right: parent.right; margins: Theme.paddingMedium; verticalCenter: parent.verticalCenter }

                        Image {
                            source: "image://theme/icon-m-%1?%2".arg (modelData ["icon"]).arg (mousearea.dynamicColor)
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Label {
                            anchors { left: parent.left; right: parent.right }
                            text: modelData["label"]
                            color: mousearea.dynamicColor
                            horizontalAlignment: Text.AlignHCenter
                            font.family: Theme.fontFamilyHeading
                            wrapMode: Text.Wrap
                        }
                    }
                }
            }
        }
    }
}
