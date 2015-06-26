import QtQuick 2.1
import harbour.sailorgram.Telegram 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../js/Settings.js" as Settings

QtObject
{
    id: settings

    property TelegramLocalStorage telegramlocalstorage: TelegramLocalStorage { }

    property Telegram telegram: Telegram {
        defaultHostAddress: "149.154.167.50"
        defaultHostDcId: 2
        defaultHostPort: 443
        appId: 27782
        appHash: "5ce096f34c8afab871edce728e6d64c9"
        configPath: telegramlocalstorage.telegramConfigPath
        publicKeyFile: telegramlocalstorage.telegramPublicKey

        onPhoneNumberChanged: {
            var phonenumber = Settings.get("phonenumber");

            if(phonenumber !== false)
                return;

            Settings.set("phonenumber", settings.telegram.phoneNumber); // Save Phone Number for fast login
        }

        onAuthSignInErrorChanged: {
            console.log("auth_error_signin");
        }

        onAuthLoggedInChanged: {
            if(!settings.telegram.authLoggedIn)
                return;

            pageStack.completeAnimation();
            pageStack.replace(Qt.resolvedUrl("../pages/conversations/ConversationsPage.qml"), { "telegram": settings.telegram })
        }

        onAuthCodeRequested: {
            pageStack.completeAnimation();
            pageStack.replace(Qt.resolvedUrl("../pages/AuthorizationPage.qml"), { "telegram": settings.telegram })
        }

        onErrorChanged: {
            console.log("!!!!!!!!!!!!!!" + error);
        }
    }
}
