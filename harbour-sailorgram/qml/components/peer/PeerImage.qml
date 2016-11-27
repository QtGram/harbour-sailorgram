import QtQuick 2.1
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0

PeerView
{
    property color backgroundColor: Theme.secondaryHighlightColor
    property color foregroundColor: Theme.primaryColor
    property int fontPixelSize: Theme.fontSizeLarge

    id: peerimage

    delegate: Item {
        id: delegateitem
        anchors.fill: parent
        layer.enabled: peerimage.hasThumbnail

        layer.effect: OpacityMask {
            maskSource: Rectangle {
                width: delegateitem.width
                height: delegateitem.height
                radius: width * 0.5
            }
        }

        Image {
            id: image
            anchors.fill: parent
            asynchronous: true
            fillMode: Image.PreserveAspectFit
            source: peerimage.thumbnail
            visible: peerimage.hasThumbnail
        }

        Rectangle {
            id: fallbackitem
            anchors.fill: parent
            color: peerimage.backgroundColor
            radius: width * 0.5
            visible: !peerimage.hasThumbnail

            Label {
                id: fallbacktext
                font { bold: true; pixelSize: peerimage.fontPixelSize }
                text: peerimage.fallbackText
                anchors.centerIn: parent
                color: peerimage.foregroundColor
            }
        }
    }
}
