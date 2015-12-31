import QtQuick 2.1
import Sailfish.Silica 1.0
import "../components"
import "../models"

Page
{
    property Context context

    property list<QtObject> translatormodel: [ QtObject { readonly property string language: qsTr("Czech");
                                                          readonly property string translators: "PawelSpoon"; },

                                               QtObject { readonly property string language: qsTr("Danish");
                                                          readonly property string translators: "flywheeldk"; },

                                               QtObject { readonly property string language: qsTr("Dutch");
                                                          readonly property string translators: "Vistaus"; },

                                               QtObject { readonly property string language: qsTr("Finnish");
                                                          readonly property string translators: "tattuh, ari_jarvio, reviewjolla"; },

                                               QtObject { readonly property string language: qsTr("French");
                                                          readonly property string translators: "Kwentin, Jordi, objectifnul, Nerfiaux, milamber"; },

                                               QtObject { readonly property string language: qsTr("German");
                                                          readonly property string translators: "Mee_Germany_Go, PawelSpoon, jupaja, kormarun, mdosch, nightmare, yoru, flacer"; },

                                               QtObject { readonly property string language: qsTr("Italian");
                                                          readonly property string translators: "Watchmaker, Dax89, dallo28, koenigseggCCGT, massimo.grigolin"; },

                                               QtObject { readonly property string language: qsTr("Persian");
                                                          readonly property string translators: "najafi"; },

                                               QtObject { readonly property string language: qsTr("Swedish");
                                                          readonly property string translators: "eson"; },

                                               QtObject { readonly property string language: qsTr("Galician");
                                                          readonly property string translators: "pakoR"; },

                                               QtObject { readonly property string language: qsTr("Russian");
                                                          readonly property string translators: "gri4994, lewa, iKozzz"; },

                                               QtObject { readonly property string language: qsTr("Belaruian");
                                                          readonly property string translators: "iKozzz"; },

                                               QtObject { readonly property string language: qsTr("Spanish");
                                                          readonly property string translators: "carmenfdezb, massimo.grigolin"; },

                                               QtObject { readonly property string language: qsTr("Greek");
                                                          readonly property string translators: "pmelas"; } ]

    id: aboutpage
    allowedOrientations: defaultAllowedOrientations

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        Column
        {
            id: content
            width: parent.width

            PageHeader
            {
                id: pageheader
                title: qsTr("About SailorGram")
            }

            Image
            {
                id: sglogo
                anchors.horizontalCenter: parent.horizontalCenter
                width: 86
                height: 86
                source: "qrc:///res/telegram.png"
            }

            Label
            {
                id: sgswname
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                text: "SailorGram"
            }

            Label
            {
                id: sgversion
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("Version") + " " + context.version
            }

            Item
            {
                width: parent.width
                height: Theme.paddingLarge + sginfo.height

                Label
                {
                    id: sginfo
                    anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall; topMargin: Theme.paddingLarge; rightMargin: Theme.paddingSmall }
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Theme.fontSizeExtraSmall
                    wrapMode: Text.WordWrap
                    text: qsTr("An unofficial Telegram Client for SailfishOS")
                }
            }

            Item
            {
                width: parent.width
                height: Theme.paddingLarge + lbldev.height + lblicondesigner.height + lblrepository.height + lbltranslationplatform.height + lblemojpack.height

                InfoLabel
                {
                    id: lbldev
                    anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall; topMargin: Theme.paddingLarge; rightMargin: Theme.paddingSmall }
                    title: qsTr("Developer")
                    text: "Antonio Davide Trogu"
                }

                InfoLabel
                {
                    id: lblicondesigner
                    anchors { left: parent.left; top: lbldev.bottom; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                    title: qsTr("Icon Designer")
                    text: "gri4994"
                }

                InfoLabel
                {
                    id: lblrepository
                    anchors { left: parent.left; top: lblicondesigner.bottom; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                    title: qsTr("GitHub Repository")
                    contentFont.underline: true
                    text: "https://github.com/Dax89/harbour-sailorgram";
                    onInfoClicked: Qt.openUrlExternally(text)
                }

                InfoLabel
                {
                    id: lbltranslationplatform
                    anchors { left: parent.left; top: lblrepository.bottom; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                    title: qsTr("Translation Platform")
                    contentFont.underline: true
                    text: "https://www.transifex.com/projects/p/sailorgram";
                    onInfoClicked: Qt.openUrlExternally(text)
                }

                InfoLabel
                {
                    id: lblemojpack
                    anchors { left: parent.left; top: lbltranslationplatform.bottom; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                    title: qsTr("Emoji Set")
                    contentFont.underline: true
                    text: "http://emojione.com"
                    onInfoClicked: Qt.openUrlExternally(text)
                }
            }

            Item
            {
                width: parent.width
                height: sectionhdr.height

                SectionHeader
                {
                    id: sectionhdr
                    width: parent.width
                    anchors { left: parent.left; top: parent.top; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingSmall }
                    text: qsTr("Translators")
                }
            }

            Column
            {
                id: coltranslators
                width: parent.width

                Repeater
                {
                    model: translatormodel

                    delegate: InfoLabel {
                        anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingSmall; topMargin: Theme.paddingMedium; rightMargin: Theme.paddingSmall }
                        title: language
                        text: translators
                        labelWrap: Text.WordWrap
                    }
                }
            }
        }
    }
}
