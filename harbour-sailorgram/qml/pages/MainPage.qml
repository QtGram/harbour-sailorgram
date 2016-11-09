import QtQuick 2.1
import Sailfish.Silica 1.0
import "../model"
import "../js/Settings.js" as Settings

Page
{
    property Context context
    property bool loggedIn: false

    Connections
    {
        target: context.telegram
        onSignInRequested: loader.setSource(Qt.resolvedUrl("../components/login/SignIn.qml"), { context: mainpage.context });
        onSignUpRequested: loader.setSource(Qt.resolvedUrl("../components/login/SignUp.qml"), { context: mainpage.context });
        onSessionPasswordNeeded: loader.setSource(Qt.resolvedUrl("../components/login/TwoFactor.qml"), { context: mainpage.context, hint: hint })

        onLoginCompleted: {
            loader.setSource(Qt.resolvedUrl("../components/dialog/DialogsList.qml"), { context: mainpage.context });
            mainpage.loggedIn = true;
        }
    }

    id: mainpage
    allowedOrientations: Orientation.All

    Component.onCompleted: {
        context.mainPage = mainpage;

        Settings.load(function(tx) {
            var phonenumber = Settings.transactionGet(tx, "phonenumber");

            if(phonenumber !== false) {
                context.telegram.initializer.phoneNumber = phonenumber;
                return;
            }

            loader.setSource(Qt.resolvedUrl("../components/login/PhoneNumber.qml"), { context: mainpage.context });
        });
    }

    SilicaFlickable
    {
        PullDownMenu
        {
            visible: mainpage.loggedIn
            busy: context.telegram.syncing

            MenuItem
            {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("settings/SettingsPage.qml"), { context: mainpage.context });
            }

            MenuItem
            {
                text: qsTr("New Group/Channel")
                enabled: false
                onClicked: pageStack.push(Qt.resolvedUrl("dialog/NewChatPage.qml"), { context: mainpage.context });
            }

            MenuItem
            {
                text: qsTr("Contacts")
                onClicked: pageStack.push(Qt.resolvedUrl("contact/ContactsPage.qml"), { context: mainpage.context });
            }
        }

        anchors.fill: parent

        PageHeader
        {
            id: header

            title: {
                if(!mainpage.loggedIn)
                    return "";

                if(!context.telegram.connected)
                    return qsTr("Connecting...");

                if(context.telegram.syncing)
                    return qsTr("Syncing...");

                return qsTr("Chats");
            }
        }

        Loader
        {
            id: loader
            anchors { left: parent.left; top: header.bottom; right: parent.right; bottom: parent.bottom }
        }
    }
}
