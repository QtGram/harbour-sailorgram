import QtQuick 2.0
import QtQuick.Controls 1.3
import TelegramQml 1.0

Item {
    id: stickerlist

    property Telegram telegram

    StickersModel {
        id: sticker_model
        telegram: stickerlist.telegram
    }

    TextField {
        id: input
        width: parent.width
        placeholderText: qsTr("Sticker short name")
        onAccepted: sticker_model.currentStickerSet = text
    }

    ScrollView {
        width: parent.width
        height: parent.height
        anchors.top: input.bottom
        anchors.bottom: parent.bottom

        GridView {
            anchors.fill: parent
            model: sticker_model
            delegate: sticker_item_component
            cellWidth: 100
            cellHeight: 100
        }
    }

    Component {
        id: sticker_item_component
        Item {
            id: item
            width: 100
            height: 100

            property Document document: model.document

            FileHandler {
                id: handler
                target: item.document
                telegram: stickerlist.telegram
                Component.onCompleted: download()
            }

            Image {
                anchors.fill: parent
                anchors.margins: 4
                height: parent.height
                width: height
                fillMode: Image.PreserveAspectFit
                sourceSize: Qt.size(width, height)
                source: handler.downloaded? handler.filePath : handler.thumbPath
            }
        }
    }
}

