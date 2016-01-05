import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.DBus 1.0
import harbour.sailorgram.SailorGram 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../js/Settings.js" as Settings
import "../js/TelegramHelper.js" as TelegramHelper
import "../js/TextElaborator.js" as TextElaborator

QtObject
{
    id: context

    readonly property string version: "0.73"
    property bool sendwithreturn: false
    property bool backgrounddisabled: false
    property bool chatheaderhidden: false
    property bool bubbleshidden: false

    property ScreenBlank screenblank: ScreenBlank { }
    property ContactsModel contacts: ContactsModel { }
    property ErrorsModel errors: ErrorsModel { }

    property SailorGram sailorgram: SailorGram {
        telegram: context.telegram
        interval: 8000 // 8 seconds
    }

    property Telegram telegram: Telegram {
        defaultHostAddress: "149.154.167.50"
        defaultHostDcId: 2
        defaultHostPort: 443
        appId: 27782
        appHash: "5ce096f34c8afab871edce728e6d64c9"
        configPath: sailorgram.configPath
        publicKeyFile: sailorgram.publicKey
        globalMute: false
        autoCleanUpMessages: true
        autoAcceptEncrypted: true

        onErrorSignal: errors.addError(errorCode, functionName, errorText)

        onIncomingMessage: {
            var elaboratedtext = TextElaborator.elaborateNotify(TelegramHelper.messageContent(msg), sailorgram.emojiPath, Theme.fontSizeSmall);
            var user = context.telegram.user(msg.fromId);

            sailorgram.notify(msg, TelegramHelper.completeName(user), elaboratedtext);
        }

        onConnectedChanged: {
            if(connected) {
               context.contacts.telegram = context.telegram;
               context.sailorgram.keepRunning = true;
            }
        }

        onPhoneNumberChanged: {
            var phonenumber = Settings.get("phonenumber");

            if(phonenumber !== false)
                return;

            Settings.set("phonenumber", context.telegram.phoneNumber); // Save Phone Number for fast login
        }

        onAuthSignInErrorChanged: {
            if(!context.telegram.authSignInError)
                return;

            pageStack.completeAnimation();
            pageStack.replace(Qt.resolvedUrl("../pages/login/AuthorizationPage.qml"), { "context": context, "authError": context.telegram.authSignInError });
        }

        onAuthSignUpErrorChanged: {
            if(!context.telegram.authSignUpError)
                return;

            pageStack.completeAnimation();
            pageStack.replace(Qt.resolvedUrl("../pages/login/SignUpPage.qml"), { "context": context, "authError": context.telegram.authSignUpError });

        }

        onAuthLoggedInChanged: {
            if(!context.telegram.authLoggedIn)
                return;

            context.telegram.online = true;

            pageStack.completeAnimation();
            pageStack.replace(Qt.resolvedUrl("../pages/dialogs/DialogsPage.qml"), { "context": context });
        }

        onAuthCodeRequested: {
            if(pageStack.currentPage.authorizationPage === true)
                return;

            pageStack.completeAnimation();

            if(context.telegram.authPhoneRegistered)
                pageStack.replace(Qt.resolvedUrl("../pages/login/AuthorizationPage.qml"), { "context": context, "authError": context.telegram.authSignInError });
            else
                pageStack.replace(Qt.resolvedUrl("../pages/login/SignUpPage.qml"), { "context": context, "authError": context.telegram.authSignUpError });
        }
    }
}
