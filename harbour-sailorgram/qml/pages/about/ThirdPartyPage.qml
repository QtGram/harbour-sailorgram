import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.Model 1.0
import "../../components"

Page
{
    id: thirdpartypage
    allowedOrientations: Orientation.Portrait

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        Column
        {
            id: content
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader
            {
                id: pageheader
                title: qsTr("Third Party")
            }

            ThirdPartyLabel
            {
                title:"Libqtelegram-ae"
                copyright: qsTr("The GPLv3 license")
                licenselink: "https://raw.githubusercontent.com/Aseman-Land/libqtelegram-aseman-edition/master/LICENSE"
                link: "https://github.com/Aseman-Land/libqtelegram-aseman-edition"
            }

            ThirdPartyLabel
            {
                title:"TelegramQML"
                copyright: qsTr("The GPLv3 license")
                licenselink: "https://raw.githubusercontent.com/Aseman-Land/TelegramQML/master/LICENSE"
                link: "https://github.com/Aseman-Land/TelegramQML"
            }

            ThirdPartyLabel
            {
                title:"Emoji One"
                copyright: qsTr("Attribution 4.0 International (CC BY 4.0)")
                licenselink: "http://emojione.com/licensing/"
                link: "http://emojione.com"
            }
        }
    }
}
