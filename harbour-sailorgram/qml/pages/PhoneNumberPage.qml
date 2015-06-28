import QtQuick 2.1
import Sailfish.Silica 1.0
import "../models"
import "../js/CountryList.js" as CountryList
import "../js/TelegramHelper.js" as TelegramHelper

Dialog
{
    property var telegram

    id: dlgphonenumber
    allowedOrientations: defaultAllowedOrientations
    acceptDestinationAction: PageStackAction.Replace
    canAccept: tfphonenumber.text.length > 0

    acceptDestination: Component {
        ConnectionPage {
            telegram: dlgphonenumber.telegram
        }
    }

    onAccepted: {
        telegram.phoneNumber = TelegramHelper.completePhoneNumber(tfcountrycode.text + tfphonenumber.text);
    }

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height

        Column
        {
            id: content
            width: parent.width

            DialogHeader
            {
                acceptText: qsTr("Next");
            }

            Item
            {
                width: parent.width
                height: lblsailorgram.contentHeight + imglogo.height + lblinfo.contentHeight + (Theme.paddingLarge * 2)

                Label
                {
                    id: lblsailorgram
                    text: "SailorGram"
                    font.bold: true
                    font.pixelSize: Theme.fontSizeHuge
                    color: Theme.secondaryHighlightColor
                    anchors { left: parent.left; top: parent.top; right: parent.right }
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }

                Image
                {
                    id: imglogo
                    source: "qrc:///res/telegram.png"
                    anchors { top: lblsailorgram.bottom; topMargin: Theme.paddingLarge; horizontalCenter: parent.horizontalCenter }
                }

                Label
                {
                    id: lblinfo
                    text: qsTr("Insert your phone number and press 'Next'")
                    font.pixelSize: Theme.fontSizeSmall
                    anchors { left: parent.left; top: imglogo.bottom; right: parent.right; topMargin: Theme.paddingLarge; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }
            }

            ComboBox
            {
                id: cbxcountrycodes
                width: parent.width
                label: qsTr("Country Code")

                menu: ContextMenu {
                    Repeater {
                        model: CountryList.countries

                        MenuItem {
                            text: CountryList.countries[index].country
                        }
                    }
                }

                onCurrentItemChanged: {
                    tfcountrycode.text = "+" + CountryList.countries[currentIndex].code;
                }
            }

            Row
            {

                id: row
                width: parent.width

                TextField
                {
                    id: tfcountrycode
                    width: font.pixelSize * 5
                    inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly
                }

                TextField
                {
                    id: tfphonenumber
                    width: parent.width - tfcountrycode.width
                    placeholderText: qsTr("Phone Number")
                    inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly
                }
            }
        }
    }
}
