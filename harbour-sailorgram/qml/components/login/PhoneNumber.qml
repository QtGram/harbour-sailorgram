import QtQuick 2.1
import Sailfish.Silica 1.0
import "../../model"
import "../../js/CountryList.js" as CountryList

Column
{
    property Context context

    id: phonenumber
    spacing: Theme.paddingLarge

    Item { width: parent.width; height: Theme.paddingLarge }

    Image
    {
        id: imglogo
        source: "qrc:///res/telegram.png"
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Label
    {
        id: lblinfo
        text: qsTr("Insert your phone number and press 'Next'")
        font.pixelSize: Theme.fontSizeSmall
        anchors { left: parent.left; right: parent.right; leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium }
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
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

    Button
    {
        id: btnnext
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Next")

        onClicked: {
            btnnext.enabled = false;
            btnnext.text = qsTr("Sending request...");

            context.telegram.initializer.phoneNumber = tfcountrycode.text + tfphonenumber.text;
        }
    }
}
