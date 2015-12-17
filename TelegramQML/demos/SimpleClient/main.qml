import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import TelegramQml 1.0
import "../share"

ApplicationWindow {
    title: qsTr("TgQml Demo: SimpleClient")
    width: 640
    height: 480
    visible: true

    Telegram {
        id: tg
        defaultHostAddress: "149.154.167.50"
        defaultHostDcId: 2
        defaultHostPort: 443
        appId: 13682
        appHash: "de37bcf00f4688de900510f4f87384bb"
        configPath: tg.homePath + "/.config/TelegramQml/examples"
        publicKeyFile: "../share/tg-server.pub"
        Component.onCompleted: setLogLevel(Telegram.LogLevelUseful)
    }

    MainForm {
        anchors.fill: parent
        telegram: tg
        visible: tg.authLoggedIn
    }

    LoginForm {
        id: login
        anchors.fill: parent
        telegram: tg
        visible: !tg.authLoggedIn
    }
}
