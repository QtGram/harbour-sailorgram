import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import "../../components/custom"
import "../../model"

Page
{
    property Context context
    property var dialog

    id: detailspage
    allowedOrientations: defaultAllowedOrientations

    PeerProfile { id: peerprofile; telegram: context.telegram; peer: detailspage.dialog }

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        Column
        {
            id: content
            width: parent.width
            spacing: Theme.paddingMedium

            PageHeader { title: qsTr("%1's profile").arg(peerprofile.title) }

            PeerImage
            {
                id: peerimage
                anchors.horizontalCenter: parent.horizontalCenter
                size: Screen.width * 0.3
                backgroundColor: Theme.secondaryHighlightColor
                foregroundColor: Theme.primaryColor
                fontPixelSize: Theme.fontSizeLarge
                peer: detailspage.dialog
            }

            Label
            {
                id: lbltitle
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                horizontalAlignment: Text.AlignHCenter
                font.family: Theme.fontFamilyHeading
                font.bold: true
                elide: Text.ElideRight

                text: {
                    if(peerprofile.hasUsername)
                        return peerprofile.title + " (@" + peerprofile.username + ")";

                    return peerprofile.title;
                }
            }

            Label
            {
                id: lblstatus
                anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.highlightColor
                elide: Text.ElideRight
                text: peerprofile.statusText
            }

            ClickableLabel
            {
                width: parent.width
                height: Theme.itemSizeSmall
                text: peerprofile.isMuted ? qsTr("Enable notifications") : qsTr("Disable notifications")

                onActionRequested: {
                    peerprofile.isMuted = !peerprofile.isMuted;
                }
            }
        }
    }
}
