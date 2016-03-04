import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Model 1.0
import "../../components"
import "../../models"

Page
{
    property Context context

    id: translationspage
    allowedOrientations: Orientation.Portrait

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        VerticalScrollDecorator { flickable: parent }

        PullDownMenu
        {
            MenuItem
            {
                text: qsTr("Translation Platform")
                onClicked: Qt.openUrlExternally("https://www.transifex.com/projects/p/sailorgram")
            }
        }

        Column
        {
            id: content
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader
            {
                id: pageheader
                title: qsTr("Translations")
            }

            Repeater
            {
                id: contentview
                model: context.sailorgram.translations

                delegate: Column {
                    anchors { left: parent.left; right: parent.right }
                    spacing: Theme.paddingMedium

                    Label {
                        anchors { left: parent.left; right: parent.right }
                        horizontalAlignment: Text.AlignHCenter
                        text: model.modelData.name
                        font.bold: true
                    }

                    CollaboratorsLabel {
                        title: qsTr("Coordinators");
                        labeldata: model.modelData.coordinators
                    }

                    CollaboratorsLabel {
                        title: qsTr("Translators");
                        labeldata: model.modelData.translators
                    }

                    CollaboratorsLabel {
                        title: qsTr("Reviewers");
                        labeldata: model.modelData.reviewers
                    }
                }
            }
        }
    }
}
