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

    property TelegramLocalStorage telegramlocalstorage: TelegramLocalStorage { }
    property Notifications notifications: Notifications { }
    property Dialog foregroundDialog: telegram.nullDialog
    property Message lastMessage: telegram.nullMessage

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

        onConnectedChanged: {
            if(connected)
               context.heartbeat.start();
        }

        onIncomingMessage: {
            var userdata = context.telegram.userData;

            if(userdata.isMuted(msg.fromId)) /* Respect Notification Settings */
                return;

            var user = context.telegram.user(msg.fromId);
            var currentpeerid = foregroundDialog !== context.telegram.nullDialog ? TelegramHelper.peerId(foregroundDialog.peer) : 0;

            if((msg.fromId !== currentpeerid) || (Qt.application.state !== Qt.ApplicationActive)) {
                notifications.send(TelegramHelper.userName(user), TelegramHelper.messageContent(msg), user.photo.photoSmall.download.location, true, false);
                return;
            }

            context.lastMessage = msg;
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
            pageStack.replace(Qt.resolvedUrl("../pages/AuthorizationPage.qml"), { "telegram": context.telegram, "authError": context.telegram.authSignInError });
        }

        onAuthSignUpErrorChanged: {
            if(!context.telegram.authSignUpError)
                return;

            pageStack.completeAnimation();
            pageStack.replace(Qt.resolvedUrl("../pages/SignUpPage.qml"), { "telegram": context.telegram, "authError": context.telegram.authSignUpError });

        }

        onAuthLoggedInChanged: {
            if(!context.telegram.authLoggedIn)
                return;

            context.telegram.online = true;

            pageStack.completeAnimation();
            pageStack.replace(Qt.resolvedUrl("../pages/conversations/ConversationsPage.qml"), { "context": context, "telegram": context.telegram });
        }

        onAuthCodeRequested: {
            pageStack.completeAnimation();

            if(context.telegram.authPhoneRegistered)
                pageStack.replace(Qt.resolvedUrl("../pages/AuthorizationPage.qml"), { "telegram": context.telegram, "authError": context.telegram.authSignInError });
            else
                pageStack.replace(Qt.resolvedUrl("../pages/SignUpPage.qml"), { "telegram": context.telegram, "authError": context.telegram.authSignUpError });
        }
    }
}
