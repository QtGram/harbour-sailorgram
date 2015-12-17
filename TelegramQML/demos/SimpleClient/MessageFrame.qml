import QtQuick 2.0
import QtQuick.Controls 1.3
import TelegramQml 1.0

Rectangle {
    id: msgFrame
    width: 100
    height: 62

    property Telegram telegram
    property Dialog dialog: telegram.nullDialog

    ScrollView {
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: send_row.top

        ListView {
            id: listv
            anchors.fill: parent
            verticalLayoutDirection: ListView.BottomToTop
            model: MessagesModel {
                telegram: msgFrame.telegram
                dialog: msgFrame.dialog
            }
            delegate: Item {
                id: item
                width: listv.width
                height: row.height + 10

                property Message message: model.item
                property Dialog dialog: telegram.dialog(message.fromId)
                
                Rectangle {
                    width: parent.width; height: 1
                    color: "#dddddd"
                }

                Row {
                    id: row
                    width: parent.width - 10
                    anchors.centerIn: parent
                    layoutDirection: item.message.out? Qt.RightToLeft : Qt.LeftToRight
                    spacing: 10

                    Image {
                        anchors.verticalCenter: parent.verticalCenter
                        height: 36; width: height
                        sourceSize: Qt.size(width, height)
                        source: handler.thumbPath
                    }

                    Text {
                        width: parent.width - 60
                        text: item.message.message
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                FileHandler {
                    id: handler
                    target: item.dialog
                    telegram: msgFrame.telegram
                    defaultThumbnail: "user.png"
                }
            }
        }
    }

    Row {
        id: send_row
        width: parent.width
        anchors.bottom: parent.bottom

        TextField {
            id: msg
            width: parent.width - send_btn.width
            anchors.verticalCenter: parent.verticalCenter
            onAccepted: send_btn.clicked()
        }

        Button {
            id: send_btn
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("SEND")
            onClicked: {
                var dialogId = dialog.peer.chatId
                if(!dialogId)
                    dialogId = dialog.peer.userId
                    
                telegram.sendMessage(dialogId, msg.text)
                msg.text = ""
            }
        }
    }
}

