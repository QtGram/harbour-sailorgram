import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../components"
import "../../models"

Page
{
    property Context context

    id: aboutpage
    allowedOrientations: Orientation.Portrait

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        PullDownMenu
        {
            MenuItem
            {
                text: qsTr("GitHub Repository")
                onClicked: Qt.openUrlExternally("https://github.com/Dax89/harbour-sailorgram")
            }

            MenuItem
            {
                text: qsTr("Report an Issue")
                onClicked: Qt.openUrlExternally("https://github.com/Dax89/harbour-sailorgram/issues")
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
                title: qsTr("About SailorGram")
            }

            Image
            {
                id: sglogo
                anchors.horizontalCenter: parent.horizontalCenter
                source: "qrc:///res/telegram.png"
            }

            Column
            {
                anchors { left: parent.left; right: parent.right }

                Label
                {
                    id: sgswname
                   anchors { left: parent.left; right: parent.right }
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                    font.pixelSize: Theme.fontSizeLarge
                    text: "SailorGram"
                }

                Label
                {
                    id: sginfo
                    anchors { left: parent.left; right: parent.right }
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Theme.fontSizeSmall
                    wrapMode: Text.WordWrap
                    text: qsTr("An unofficial Telegram Client for SailfishOS")
                }

                Label
                {
                    id: sgversion
                    anchors { left: parent.left; right: parent.right }
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.secondaryColor
                    text: qsTr("Version") + " " + context.versionString()
                }

                Label
                {
                    id: sgcopyright
                    anchors { left: parent.left; right: parent.right }
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Theme.fontSizeExtraSmall
                    wrapMode: Text.WordWrap
                    color: Theme.secondaryColor
                    text: qsTr("SailorGram is distributed under the GPLv3 license")
                }
            }

            Column
            {
                anchors { left: parent.left; right: parent.right; topMargin: Theme.paddingExtraLarge }
                spacing: Theme.paddingSmall

                Button
                {
                    id: licensebutton
                    text: qsTr("License")
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: Qt.openUrlExternally("https://raw.githubusercontent.com/Dax89/harbour-sailorgram/master/LICENSE")
                }

                Button
                {
                    id: developersbutton
                    text: qsTr("Developers")
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: pageStack.push(Qt.resolvedUrl("DevelopersPage.qml"))
                }

                Button
                {
                    id: translationsbutton
                    text: qsTr("Translations")
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: pageStack.push(Qt.resolvedUrl("TranslationsPage.qml"), { "context": aboutpage.context })
                }

                Button
                {
                    id: thirdpartybutton
                    text: qsTr("Third Party")
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: pageStack.push(Qt.resolvedUrl("ThirdPartyPage.qml"))
                }
            }
        }
    }
}
