import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../../model"

Page
{
    property Context context

    id: newchannelpage

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        PullDownMenu
        {
            MenuItem
            {
                text: qsTr("Create channel")
                enabled: (tfname.text.length > 0) && (tfdescription.text.length > 0)

                onClicked: {

                }
            }
        }

        Column
        {
            id: content
            width: parent.width

            PageHeader { title: qsTr("New Channel") }

            TextField
            {
                id: tfname
                x: Theme.horizontalPageMargin
                width: parent.width - (x * 2)
                placeholderText: qsTr("Name")
            }

            TextField
            {
                id: tfdescription
                x: Theme.horizontalPageMargin
                width: parent.width - (x * 2)
                placeholderText: qsTr("Description")
            }
        }
    }
}
