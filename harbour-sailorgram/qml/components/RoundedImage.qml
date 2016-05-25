import QtQuick 2.1
import QtGraphicalEffects 1.0
import harbour.sailorgram.TelegramQml 2.0 as Telegram

Item
{
    property alias imageHandler: tlimage

    id: roundedimage

    Telegram.Image
    {
        id: tlimage
        qtQuickVersion: "2.1"
        anchors.fill: parent
        visible: false
    }

    Rectangle
    {
        id: mask
        anchors.fill: parent
        radius: width * 0.5
        visible: false
        color: "black"
    }

    OpacityMask
    {
        anchors.fill: mask
        source: imageHandler
        maskSource: mask
    }
}

