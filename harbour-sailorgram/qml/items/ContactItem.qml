import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0

Item
{
    property User user
    property alias currentStatus: lblstatus.text

    id: contactitem

    Component.onCompleted: {
        if(!user.photo.photoSmall.download.downloaded)
            telegram.getFile(user.photo.photoSmall);
    }

    Row
    {
        anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
        spacing: Theme.paddingSmall

        Image
        {
            id: imgcontact
            width: contactitem.height
            height: contactitem.height
            fillMode: Image.PreserveAspectFit
            cache: false
            source: user.photo.photoSmall.download.location
        }

        Column
        {
            width: parent.width - imgcontact.width
            height: parent.height

            Label
            {
                id: lblfullname
                width: parent.width
                elide: Text.ElideRight
                text: ((user.username.length > 0) ? user.username : (user.firstName + " " + user.lastName))
            }

            Row
            {
                height: contactitem.height - lblfullname.contentHeight

                Label {
                    id: lblstaticstatus
                    text: qsTr("Status") + ": " + user.status.wasOnline ? qsTr("Online") : qsTr("Offline")
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.highlightColor
                }

                Label {
                    id: lblstatus
                    width: contactitem.width - lblstaticstatus.contentWidth
                    font.pixelSize: Theme.fontSizeExtraSmall
                    elide: Text.ElideRight
                }
            }
        }
    }
}
