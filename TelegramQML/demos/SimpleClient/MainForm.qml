import QtQuick 2.4
import QtQuick.Controls 1.3
import TelegramQml 1.0

Item {
    id: mainForm
    width: 640
    height: 480

    property Telegram telegram

    DialogList {
        id: dialogList
        height: parent.height
        telegram: mainForm.telegram
    }

    MessageFrame {
        width: parent.width - dialogList.width
        height: parent.height
        anchors.right: parent.right
        telegram: mainForm.telegram
        dialog: dialogList.currentDialog
    }
}
