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
    cacheBuffer: Screen.height * 2
    verticalLayoutDirection: ListView.BottomToTop
    currentIndex: -1
    clip: true
    Component.onCompleted: messageslist.positionViewAtIndex(model.newMessageIndex, ListView.Center);

    header: Column {
        width: messageslist.width
        spacing: Theme.paddingMedium
        Component.onCompleted: messageslist.scrollToBottom()


        MessageTextInput {
            id: messagetextinput
            width: parent.width
            onSendMessage: model.sendMessage(message)
        }
    }

    delegate: MessageModelItem {
        width: parent.width
        maxWidth: parent.width * 0.8
    }

    VerticalScrollDecorator { flickable: messageslist }
}
