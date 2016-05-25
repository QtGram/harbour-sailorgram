import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../models"
import "../peer"
import "../../js/TelegramHelper.js" as TelegramHelper

Item
{

    property alias statusText: lblstaticstatus.text
    property Context context
    property User user
    property bool showUsername: false

    id: useritem

    PeerImage
    {
        id: useravatar
        anchors { left: parent.left; top: parent.top }
        width: useritem.height
        height: useritem.height
        context: useritem.context
        peer: useritem.user
        fallbackTitle: TelegramHelper.completeName(user)
    }

    Column
    {
        anchors { left: useravatar.right; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall }

        Label
        {
            id: lblfullname
            width: parent.width
            elide: Text.ElideRight
            text: TelegramHelper.completeName(user)
        }

        Label
        {
            id: lblusername
            width: parent.width
            elide: Text.ElideRight
            text: (user.username.length > 0) ? "@" + user.username : ""
            visible: showUsername && (text.length > 0)
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryHighlightColor
        }

        Row
        {
            height: useritem.height - lblfullname.contentHeight

            Label {
                id: lblstaticstatus
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.highlightColor
            }

            Label {
                id: lblstatus
                width: useritem.width - lblstaticstatus.contentWidth
                font.pixelSize: Theme.fontSizeExtraSmall
                elide: Text.ElideRight
            }
        }
    }
}
