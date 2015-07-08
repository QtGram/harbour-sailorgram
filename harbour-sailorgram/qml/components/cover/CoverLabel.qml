import QtQuick 2.1
import Sailfish.Silica 1.0

Row
{
    property alias descriptionText: lbldescription.text
    property alias valueText: lblvalue.text
    property alias valueColor: lblvalue.color

    id: coverlabel
    height: Math.max(lbldescription.contentHeight, lblvalue.contentHeight)
    spacing: Theme.paddingSmall

    Label
    {
        id: lbldescription
        color: Theme.secondaryColor
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: Theme.fontSizeExtraSmall
        wrapMode: Text.NoWrap
    }

    Label
    {
        id: lblvalue
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: Theme.fontSizeExtraSmall
        wrapMode: Text.NoWrap
        elide: Text.ElideRight
    }
}
