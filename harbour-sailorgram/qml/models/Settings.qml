import QtQuick 2.1
import harbour.sailorgram.DBus 1.0
import harbour.sailorgram.SailorGram 1.0
import harbour.sailorgram.Telegram 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../js/Settings.js" as Settings
import "../js/TelegramHelper.js" as TelegramHelper

QtObject
{
    id: settings

    property TelegramLocalStorage telegramlocalstorage: TelegramLocalStorage { }
    property Notifications notifications: Notifications { }
    property Dialog foregroundDialog: telegram.nullDialog
    property Message lastMessage: telegram.nullMessage

    property HeartBeat heartbeat: HeartBeat {
        telegram: settings.telegram
        interval: 8000
        onConnectedChanged: connected ? sailorgram.wake() : sailorgram.sleep();
    }

    property SailorGram sailorgram: SailorGram {
        telegram: settings.telegram
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
               settings.heartbeat.beat();
        }

        onIncomingMessage: {
            var userdata = settings.telegram.userData;

            if(userdata.isMuted(msg.fromId)) /* Respect Notification Settings */
                return;

            var user = settings.telegram.user(msg.fromId);
            var currentpeerid = foregroundDialog !== settings.telegram.nullDialog ? TelegramHelper.peerId(foregroundDialog.peer) : 0;

            if((msg.fromId !== currentpeerid) || (Qt.application.state !== Qt.ApplicationActive)) {
                notifications.send(TelegramHelper.userName(user), TelegramHelper.messageContent(msg), user.photo.photoSmall.download.location, true, false);
                return;
            }

            settings.lastMessage = msg;
            notifications.beep();
        }

        onPhoneNumberChanged: {
            var phonenumber = Settings.get("phonenumber");

            if(phonenumber !== false)
                return;

            Settings.set("phonenumber", settings.telegram.phoneNumber); // Save Phone Number for fast login
        }

        onAuthSignInErrorChanged: {
            if(!settings.telegram.authSignInError)
                return;

            pageStack.completeAnimation();
            pageStack.replace(Qt.resolvedUrl("../pages/AuthorizationPage.qml"), { "telegram": settings.telegram, "authError": settings.telegram.authSignInError });
        }

        onAuthSignUpErrorChanged: {
            if(!settings.telegram.authSignUpError)
                return;

            pageStack.completeAnimation();
            pageStack.replace(Qt.resolvedUrl("../pages/SignUpPage.qml"), { "telegram": settings.telegram, "authError": settings.telegram.authSignUpError });

        }

        onAuthLoggedInChanged: {
            if(!settings.telegram.authLoggedIn)
                return;

            settings.telegram.online = true;

            pageStack.completeAnimation();
            pageStack.replace(Qt.resolvedUrl("../pages/conversations/ConversationsPage.qml"), { "settings": settings, "telegram": settings.telegram });
        }

        onAuthCodeRequested: {
            pageStack.completeAnimation();

            if(settings.telegram.authPhoneRegistered)
                pageStack.replace(Qt.resolvedUrl("../pages/AuthorizationPage.qml"), { "telegram": settings.telegram, "authError": settings.telegram.authSignInError });
            else
                pageStack.replace(Qt.resolvedUrl("../pages/SignUpPage.qml"), { "telegram": settings.telegram, "authError": settings.telegram.authSignUpError });
        }
    }
}
