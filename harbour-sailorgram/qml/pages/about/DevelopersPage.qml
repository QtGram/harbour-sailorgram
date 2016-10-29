import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Model 1.0
import "../../components/about"

Page
{
    id: developerspage
    allowedOrientations: Orientation.Portrait

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        VerticalScrollDecorator { flickable: parent }

        Column
        {
            id: content
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader
            {
                id: pageheader
                title: qsTr("Developers")
            }

            CollaboratorsLabel {
                title: qsTr("Author");
                labelData: [ "Antonio Davide Trogu" ]
            }

            CollaboratorsLabel {
                title: qsTr("Contributors");
                labelData: [ "toxip", "thebootroo", "Maldela", "mentaljam" ]
            }

            CollaboratorsLabel {
                title: qsTr("Icon Designers");
                labelData: [ "gri4994", "toxip" ]
            }
        }
    }
}
