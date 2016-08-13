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

    readonly property string completeName: TelegramHelper.completeName(user)

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

            PageHeader { title: contactpage.completeName }

            PeerProfile
            {
                id: peerprofile
                width: parent.width
                context: contactpage.context
                peer: contactpage.user
                fallbackAvatar: contactpage.completeName
                //FIXME: StatusText

                title: {
                    var name = contactpage.completeName;

                    if(contactpage.user.username.length > 0)
                        name += " (@" + contactpage.user.username + ")";

                    return name;
                }
            }

            UserInfo
            {
                x: Theme.paddingMedium
                width: parent.width - (x * 2)
                context: contactpage.context
                user: contactpage.user
                actionVisible: true
                showHeader: true
            }
        }
    }
}
