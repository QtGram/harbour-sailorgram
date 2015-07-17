import QtQuick 2.1
import Sailfish.Silica 1.0

Item
{
    property string title
    property bool displayColon: true
    property int labelElide: Text.ElideNone
    property int labelWrap: Text.NoWrap

    property alias titleFont: lbltitle.font
    property alias contentFont: lblcontent.font
    property alias contentColor: lblcontent.color
    property alias text: lblcontent.text

    height: lbltitle.height + lblcontent.height

    Label
    {
        id: lbltitle
        anchors { left: parent.left; top: parent.top; right: parent.right }
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.highlightColor
        text: title + (displayColon ? ":" : "")
        elide: labelElide
        wrapMode: labelWrap
    }

    Label
    {
        id: lblcontent
        anchors { left: parent.left; top: lbltitle.bottom; right: parent.right}
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: Theme.fontSizeExtraSmall
        elide: labelElide
        wrapMode: labelWrap
    }
}
