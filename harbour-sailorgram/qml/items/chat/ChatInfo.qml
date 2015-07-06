import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../user"

Item
{
    property int oldParticipantsCount: -1
    property Settings settings
    property Telegram telegram
    property Dialog dialog
    property Chat chat

    id: chatinfo

    SectionHeader {
        id: hdrsection
        text: qsTr("Members")
    }

    SilicaListView
    {
        id: lvpartecipants
        spacing: Theme.paddingMedium
        anchors { left: parent.left; top: hdrsection.bottom; right: parent.right; bottom: parent.bottom }

        model: ChatParticipantsModel {
            id: chatparticipantsmodel
            telegram: chatinfo.telegram
            dialog: chatinfo.dialog
        }

        delegate: ListItem {
            property ChatParticipant participant: item
            property User user: telegram.user(participant.userId)

            id: liparticipant
            contentWidth: parent.width
            contentHeight: Theme.itemSizeSmall

            UserItem {
                id: useritem
                anchors.fill: parent
                telegram: chatinfo.telegram
                user: liparticipant.user
            }
        }
    }
}
