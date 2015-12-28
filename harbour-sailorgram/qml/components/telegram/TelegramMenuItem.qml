import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../models"

MenuItem
{
    property bool menuVisible: true
    property Context context

    id: telegrammenuitem
    visible: context.sailorgram.connected && menuVisible
}
