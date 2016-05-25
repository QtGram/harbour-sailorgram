import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../../models"
import "../../components"
import "../../items/peer"
import "../../items/user"
import "../../js/TelegramHelper.js" as TelegramHelper

Page
{
    property Context context
    property User user

    id: contactpage
    allowedOrientations: defaultAllowedOrientations

    SilicaFlickable
    {
        id: flickable
        anchors.fill: parent
        contentHeight: content.height

        Column
        {
            id: content
            width: parent.width
            spacing: Theme.paddingMedium

            PageHeader { title: TelegramHelper.completeName(user) }

            PeerProfile
            {
                id: peerprofile
                width: parent.width
                context: contactpage.context
                user: contactpage.user
                showType: false
            }

            UserInfo
            {
                x: Theme.paddingMedium
                width: parent.width - (x * 2)
                actionVisible: true
                context: contactpage.context
                user: contactpage.user
                showHeader: true
            }
        }
    }
}
