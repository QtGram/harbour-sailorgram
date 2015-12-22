import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../models"
import "../../components/telegram"

SilicaListView
{
    property Context context

    id: messageview
    currentIndex: -1
    verticalLayoutDirection: ListView.BottomToTop
    spacing: Theme.paddingLarge
    cacheBuffer: (dialogpage.isPortrait ? Screen.height : Screen.width) * 2
    clip: true

    VerticalScrollDecorator { flickable:  messageview }
    TelegramBackground { visible: !context.backgrounddisabled; z: -1 }
}
