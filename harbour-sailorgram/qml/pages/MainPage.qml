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
    allowedOrientations: defaultAllowedOrientations

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
            busy: context.telegram.syncing || context.reconnecting

            MenuItem
            {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("settings/SettingsPage.qml"), { context: mainpage.context });
            }

            MenuItem
            {
                text: qsTr("Connect now")
                visible: context.reconnecting
                onClicked: context.telegram.reconnect()
            }

            MenuItem
            {
                text: qsTr("New Chat")
                visible: context.telegram.connected

                onClicked: {
                    var newchatpage = pageStack.push(Qt.resolvedUrl("dialog/chat/NewChatPage.qml"), { context: mainpage.context });

                    newchatpage.cloudChatRequested.connect(function() {
                        pageStack.pop(mainpage, PageStackAction.Immediate);
                        context.openDialog(context.dialogs.cloudDialog, true);
                    });
                }
            }

            MenuItem
            {
                text: qsTr("Contacts")
                visible: context.telegram.connected

                onClicked: {
                    var contactspage = pageStack.push(Qt.resolvedUrl("contact/ContactsPage.qml"), { context: mainpage.context });

                    contactspage.dialogCreated.connect(function(dialog) {
                        pageStack.pop(mainpage, PageStackAction.Immediate);
                        context.openDialog(dialog, true);
                    });
                }
            }
        }

        anchors.fill: parent

        Loader
        {
            id: loader
            anchors.fill: parent
        }
    }
}
