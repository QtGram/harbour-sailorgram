import QtQuick 2.0
import QtQuick.Controls 1.3
import TelegramQml 1.0

Item {
    id: dialogList
    width: 250

    property Telegram telegram
    property Dialog currentDialog: telegram.nullDialog

    ScrollView {
        anchors.fill: parent

        ListView {
            id: listView
            anchors.fill: parent
            model: DialogsModel {
                telegram: dialogList.telegram
            }
            delegate: dialog_component
        }
    }

    Component {
        id: dialog_component
        Rectangle {
            id: item
            width: listView.width
            height: 50
            color: currentDialog==dialog? "#330d80ec" : "#00000000"

            property Dialog dialog: model.item
            property User user: telegram.user(dialog.peer.userId)
            property Chat chat: telegram.chat(dialog.peer.chatId)

            FileHandler {
                id: handler
                target: item.dialog
                telegram: dialogList.telegram
                defaultThumbnail: "user.png"
            }

            Row {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 10

                Image {
                    height: parent.height
                    width: height
                    sourceSize: Qt.size(width, height)
                    source: handler.thumbPath
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: item.dialog.peer.chatId? item.chat.title
                                                 : item.user.firstName + " " + item.user.lastName
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: currentDialog = item.dialog
            }
        }
    }
}

