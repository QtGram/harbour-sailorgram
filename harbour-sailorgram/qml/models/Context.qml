import QtQuick 2.1
import harbour.sailorgram.DBus 1.0
import harbour.sailorgram.SailorGram 1.0
import harbour.sailorgram.TelegramLocalStorage 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../js/Settings.js" as Settings
import "../js/TelegramHelper.js" as TelegramHelper

QtObject
{
    id: context

    readonly property string version: "0.73"
    property bool sendwithreturn: false
    property bool backgrounddisabled: false
    property bool chatheaderhidden: false
    property bool bubbleshidden: false

    property Dialog foregroundDialog: telegram.nullDialog
    property TelegramLocalStorage telegramlocalstorage: TelegramLocalStorage { }
    property ScreenBlank screenblank: ScreenBlank { }
    property Notifications notifications: Notifications { }
    property ContactsModel contacts: ContactsModel { }
    property ErrorsModel errors: ErrorsModel { }
    property SailorgramInterface sailorgraminterface: SailorgramInterface { }

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
        configPath: telegramlocalstorage.telegramConfigPath
        publicKeyFile: telegramlocalstorage.telegramPublicKey
        globalMute: false
        autoCleanUpMessages: true
        autoAcceptEncrypted: true

        onErrorSignal: errors.addError(errorCode, functionName, errorText)

        onConnectedChanged: {
            if(connected) {
               context.contacts.telegram = context.telegram;
               context.sailorgram.keepRunning = true;
            }
        }

        onIncomingMessage: {
            if(context.telegram.globalMute)
                return;

            var userdata = context.telegram.userData;

            if(TelegramHelper.isServiceMessage(msg)) /* Do not notify for Action Messages */
                return;

            var dialog = context.telegram.messageDialog(msg.id);
            var peerid = TelegramHelper.peerId(dialog);

            if(userdata.isMuted(peerid) || (msg.fromId === context.telegram.me)) // Don't notify myself!
                return;

            var user = context.telegram.user(msg.fromId);

            if((dialog !== foregroundDialog) || (Qt.application.state !== Qt.ApplicationActive))
                notifications.send(TelegramHelper.completeName(user), TelegramHelper.messageContent(msg), user.photo.photoSmall.download.location, true, (Qt.application.state === Qt.ApplicationActive));
            else
                notifications.beep();
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
