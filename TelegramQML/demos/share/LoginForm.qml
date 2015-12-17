import QtQuick 2.0
import QtQuick.Controls 1.3
import TelegramQml 1.0

Rectangle {
    width: 100
    height: 62
    color: myPalette.window

    property Telegram telegram

    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

    Column {
        anchors.centerIn: parent
        spacing: 4
        visible: telegram.phoneNumber.length == 0

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 20
            color: "#333333"
            text: qsTr("Enter Your Phone")
        }

        TextField {
            id: phone
            width: 300
            validator: RegExpValidator{regExp: /\+(?!0)\d*/}
            placeholderText: qsTr("ex: +123456789")
            onAccepted: login_btn.clicked()
        }

        Button {
            id: login_btn
            text: qsTr("Login")
            width: 300
            onClicked: telegram.phoneNumber = phone.text
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: telegram.phoneNumber.length != 0 &&
                 !telegram.authNeeded &&
                 !telegram.authLoggedIn
    }

    Column {
        anchors.centerIn: parent
        spacing: 4
        visible: telegram.phoneNumber.length != 0 &&
                 telegram.authNeeded

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 20
            color: "#333333"
            text: qsTr("Enter Code")
        }

        TextField {
            id: code
            width: 300
        }

        Button {
            text: qsTr("Send Code")
            width: 300
            onClicked: telegram.authSignIn(code.text)
        }
    }
}

