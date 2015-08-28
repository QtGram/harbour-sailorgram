import QtQuick 2.1
import harbour.sailorgram.DBus 1.0
import harbour.sailorgram.SailorGram 1.0
import harbour.sailorgram.Telegram 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../js/Settings.js" as Settings
import "../js/TelegramHelper.js" as TelegramHelper

QtObject
{
    id: context

    readonly property string version: "0.71"
    property bool sendwithreturn: false
    property bool backgrounddisabled: false
    property bool chatheaderhidden: false

    property TelegramLocalStorage telegramlocalstorage: TelegramLocalStorage { }
    property ScreenBlank screenblank: ScreenBlank { }
    property Notifications notifications: Notifications { }
    property ContactsModel contacts: ContactsModel { }
    property ErrorsModel errors: ErrorsModel { }
    property Dialog foregroundDialog: telegram.nullDialog

    property HeartBeat heartbeat: HeartBeat {
        telegram: context.telegram
        interval: 8000
        onConnectedChanged: connected ? sailorgram.wake() : sailorgram.sleep();
    }

    property SailorGram sailorgram: SailorGram {
        telegram: context.telegram
    }

    property Telegram telegram: Telegram {
        defaultHostAddress: "149.154.167.50"
        defaultHostDcId: 2
        defaultHostPort: 443
        appId: 27782
        appHash: "5ce096f34c8afab871edce728e6d64c9"
        configPath: telegramlocalstorage.telegramConfigPath
        publicKeyFile: telegramlocalstorage.telegramPublicKey
        autoCleanUpMessages: true

        onErrorSignal: errors.addError(errorCode, functionName, errorText)

        onConnectedChanged: {
            if(connected) {
               context.heartbeat.start();
               context.contacts.telegram = context.telegram;
            }
        }

        onIncomingMessage: {
            var userdata = context.telegram.userData;

            if(TelegramHelper.isActionMessage(msg)) /* Do not notify for Action Messages */
                return;

            var dialog = context.telegram.messageDialog(msg.id);
            var peerid = TelegramHelper.peerId(dialog);

            if(userdata.isMuted(peerid))
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
            pageStack.replace(Qt.resolvedUrl("../pages/AuthorizationPage.qml"), { "context": context, "authError": context.telegram.authSignInError });
        }

        onAuthSignUpErrorChanged: {
            if(!context.telegram.authSignUpError)
                return;

            pageStack.completeAnimation();
            pageStack.replace(Qt.resolvedUrl("../pages/SignUpPage.qml"), { "context": context, "authError": context.telegram.authSignUpError });

        }

        onAuthLoggedInChanged: {
            if(!context.telegram.authLoggedIn)
                return;

            context.telegram.online = true;

            pageStack.completeAnimation();
            pageStack.replace(Qt.resolvedUrl("../pages/conversations/ConversationsPage.qml"), { "context": context });
        }

        onAuthCodeRequested: {
            if(pageStack.currentPage.authorizationPage === true)
                return;

            pageStack.completeAnimation();

            if(context.telegram.authPhoneRegistered)
                pageStack.replace(Qt.resolvedUrl("../pages/AuthorizationPage.qml"), { "context": context, "authError": context.telegram.authSignInError });
            else
                pageStack.replace(Qt.resolvedUrl("../pages/SignUpPage.qml"), { "context": context, "authError": context.telegram.authSignUpError });
        }
    }
}
