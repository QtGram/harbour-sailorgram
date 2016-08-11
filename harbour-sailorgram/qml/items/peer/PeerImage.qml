import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../components"
import "../../models"
import "../../js/TelegramHelper.js" as TelegramHelper

RoundedImage
{
    property Context context
    property string fallbackTitle
    property var peer
    property bool showType: true
    property bool isChat: false
    property bool isBroadcast: false
    property bool isSecretChat: false

    id: imgpeer

    imageHandler {
        engine: context.engine
        source: imgpeer.peer
        fillMode: Image.PreserveAspectFit
        asynchronous: true
    }

    Rectangle
    {
        id: imgfallback
        anchors.fill: parent
        color: Theme.secondaryHighlightColor
        radius: imgpeer.width * 0.5
        visible: !imgpeer.imageHandler.thumbnailDownloaded || (imgpeer.status === Image.Error)

        Label {
            anchors.centerIn: parent
            font.bold: true
            font.pixelSize: parent.height * 0.6
            text: TelegramHelper.fallbackText(fallbackTitle)
        }
    }

    Image
    {
        id: imgpeertype
        asynchronous: true
        anchors { bottom: parent.bottom; right: parent.right }
        fillMode: Image.PreserveAspectFit
        width: Theme.iconSizeSmall
        height: Theme.iconSizeSmall
        visible: showType && (isBroadcast || isSecretChat)

        source: {
            if(isSecretChat)
                return "image://theme/icon-s-secure"

            if(isBroadcast)
                return "qrc:///res/broadcast.png";

            if(isChat)
                return "image://theme/icon-s-chat";

            return "";
        }
    }
}
