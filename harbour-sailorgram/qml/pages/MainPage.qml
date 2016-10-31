import QtQuick 2.1
import Sailfish.Silica 1.0
import "../model"
import "../js/Settings.js" as Settings

Page
{
    property Context context: Context {
        telegram {
            onSignInRequested: loader.setSource(Qt.resolvedUrl("../components/login/SignIn.qml"), { context: mainpage.context });
            onSignUpRequested: loader.setSource(Qt.resolvedUrl("../components/login/SignUp.qml"), { context: mainpage.context });

            onLoginCompleted: {
                loader.setSource(Qt.resolvedUrl("../components/dialog/DialogsList.qml"), { context: mainpage.context });
                header.title = qsTr("Chats");

                mainpage.needsContactPage = true;
                mainpage.attachContacts();
            }
        }

        Component.onCompleted: {
            Settings.load(function(tx) {
                var phonenumber = Settings.transactionGet(tx, "phonenumber");

                if(phonenumber !== false) {
                    context.telegram.initializer.phoneNumber = phonenumber;
                    return;
                }

                loader.setSource(Qt.resolvedUrl("../components/login/PhoneNumber.qml"), { context: mainpage.context });
            });
        }
    }

    property bool needsContactPage: false

    function attachContacts() {
        if(!needsContactPage || (status !== PageStatus.Active) || mainpage.canNavigateForward)
            return;

        pageStack.pushAttached(Qt.resolvedUrl("contact/ContactsPage.qml"), { context: mainpage.context });
    }

    id: mainpage
    onStatusChanged: attachContacts()

    SilicaFlickable
    {
        anchors.fill: parent

        PageHeader { id: header }

        Loader
        {
            id: loader
            anchors { left: parent.left; top: header.bottom; right: parent.right; bottom: parent.bottom }
        }
    }
}
