import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../components/telegram"
import "../../components/message"

SilicaListView
{
    property alias messagesModel: messagesmodel
    property real headerHeight
    property Context context
    property MessageTypesPool messageTypesPool: MessageTypesPool { }

    id: messageview
    currentIndex: -1
    verticalLayoutDirection: ListView.BottomToTop
    spacing: Theme.paddingLarge
    cacheBuffer: (pageStack.currentPage.isPortrait ? Screen.height : Screen.width) * 2
    clip: true

    header: Item {
        id: headeritem
        width: messageview.width
        height: headerHeight
    }

    model: MessagesModel {
        id: messagesmodel
        stepCount: context.stepcount

        onCountChanged: {
            if((count <= 0) || (Qt.application.state !== Qt.ApplicationActive))
                return;

            messagesmodel.setReaded(); /* We are in this chat, always mark these messages as read */
        }
    }

    Timer
    {
        id: refreshtimer
        repeat: true
        interval: 10000
        running: !context.sailorgram.daemonized
        onTriggered: messagesmodel.refresh()
    }

    BusyIndicator
    {
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
        running: context.sailorgram.connected && messagesmodel.refreshing && (messagesmodel.count <= 0)
        z: running ? 2 : 0
    }

    Connections
    {
        target: Qt.application

        onStateChanged: {
            if(Qt.application.state !== Qt.ApplicationActive)
                return;

            messagesmodel.setReaded();
        }
    }

    VerticalScrollDecorator { flickable:  messageview }
    TelegramBackground { visible: !context.backgrounddisabled; z: -1 }
}
