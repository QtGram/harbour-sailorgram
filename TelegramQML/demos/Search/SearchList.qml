import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import TelegramQml 1.0
import "../share"

Item {
    width: 640
    height: 480

    property alias telegram: smodel.telegram

    SearchModel {
        id: smodel
    }

    TextField {
        id: keyword_txt
        width: parent.width
        placeholderText: qsTr("Search Keyword")
        onAccepted: smodel.keyword = text
    }

    ScrollView {
        width: parent.width
        anchors.top: keyword_txt.bottom
        anchors.bottom: parent.bottom

        ListView {
            id: listView
            anchors.fill: parent
            model: smodel
            delegate: dialog_component
        }
    }

    Component {
        id: dialog_component
        Rectangle {
            id: item
            width: listView.width
            height: 50

            property Message message: model.item
            property User user: telegram.user(message.fromId)

            property Peer toPeer: message.toId
            property User toUser: telegram.user(toPeer.userId)
            property Chat toChat: telegram.chat(toPeer.chatId)

            FileHandler {
                id: handler
                target: item.user
                telegram: smodel.telegram
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

                Column {
                    anchors.verticalCenter: parent.verticalCenter

                    Text {
                        text: {
                            var from = (item.user.firstName + " " + item.user.lastName).trim()
                            var to = item.toPeer.chatId? item.toChat.title :
                                                         item.toUser.firstName +
                                                         " " + item.toUser.lastName
                            to = to.trim()
                            return ("<b>from:</b> %1 <b>to:</b> %2").arg(from).arg(to)
                        }
                    }
                    Text {
                        text: item.message.message
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        elide: Text.ElideRight
                        maximumLineCount: 1
                    }
                }
            }
        }
    }
}
