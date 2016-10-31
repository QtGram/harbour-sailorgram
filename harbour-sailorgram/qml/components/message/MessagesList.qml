import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../items"
import "../../model"

SilicaListView
{
    property alias messagesModel: messageslist.model

    id: messageslist
    spacing: Theme.paddingMedium
    cacheBuffer: model.loadCount
    verticalLayoutDirection: ListView.BottomToTop
    currentIndex: -1
    clip: true

    header: Column {
        width: messageslist.width
        spacing: Theme.paddingMedium

        Behavior on height { NumberAnimation { duration: 250 } }

        MessageTextInput {
            id: messagetextinput
            width: parent.width
            onSendMessage: model.sendMessage(message)
        }
    }

    delegate: MessageModelItem {
        maxWidth: parent.width * 0.8
    }

    VerticalScrollDecorator { flickable: messageslist }
}
