import QtQuick 2.1
import Sailfish.Silica 1.0

Item
{
    property bool errorState: false
    property alias title: lbltitle.text
    property alias text: lbltext.text

    id: debuglabel
    x: Theme.paddingMedium
    width: row.width - (Theme.paddingMedium * 2)
    height: row.height

    Row
    {
        id: row
        spacing: Theme.paddingMedium
        height: Math.max(lbltitle.contentHeight, lbltext.contentHeight)

        Label
        {
            id: lbltitle
            color: Theme.highlightColor
            font.pixelSize: Theme.fontSizeExtraSmall
            verticalAlignment: Text.AlignTop
            wrapMode: Text.NoWrap
            elide: Text.ElideRight
        }

        Label
        {
            id: lbltext
            color: debuglabel.errorState ? "red" : Theme.primaryColor
            verticalAlignment: Text.AlignTop
            font.pixelSize: Theme.fontSizeExtraSmall
            wrapMode: Text.Wrap
        }
    }
}
