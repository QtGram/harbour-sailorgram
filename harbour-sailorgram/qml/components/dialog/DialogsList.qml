import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../components/custom"
import "../../model"
import "../../items"

SilicaListView
{
    property Context context

    id: dialogslist
    spacing: Theme.paddingMedium
    model: context.dialogs
    cacheBuffer: Screen.height * 2
    clip: true

    header: PageHeader {
        id: pageheader

        title: {
            if(!mainpage.loggedIn)
                return "";

            if(context.reconnecting)
                return qsTr("Connection in %1...").arg(context.reconnectSeconds);

            if(!context.telegram.connected)
                return qsTr("Connecting...");

            if(context.telegram.syncing)
                return qsTr("Syncing...");

            return qsTr("Chats");
        }

        ConnectionStatus {
            anchors {
                verticalCenter: pageheader.extraContent.verticalCenter
                left: pageheader.extraContent.left
                leftMargin: -Theme.horizontalPageMargin
            }
        }
    }

    delegate: DialogModelItem {
        width: parent.width
        onClicked: context.openDialog(model.item)
    }
}
