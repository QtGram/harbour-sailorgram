import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.DBus 1.0
//import harbour.sailorgram.Insomniac 1.0
import harbour.sailorgram.SailorGram 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../js/Settings.js" as Settings
import "../js/TelegramHelper.js" as TelegramHelper
import "../js/TextElaborator.js" as TextElaborator

QtObject
{
    id: context

    readonly property bool beta: false
    readonly property int betanum: 1
    readonly property string version: "0.79"
    readonly property int stepcount: 25
    property bool sendwithreturn: false
    property bool backgrounddisabled: false
    property bool chatheaderhidden: false
    property bool bubbleshidden: false
    property bool angledbubbles: false
    property int bubblesopacity: 100

    function versionString() {
        var ver = context.version;

        if(beta)
            ver += " BETA " + betanum;

        return ver;
    }

    property Document currentSticker: null
    property ScreenBlank screenblank: ScreenBlank { }
    property ErrorsModel errors: ErrorsModel { }
    property StickersModel stickers: StickersModel { }
    property ContactsModel contacts: ContactsModel { }
    property DialogsModel dialogs: DialogsModel { }

    /*
    property Insomniac insomniac: Insomniac { // Keeps updating dialogs during deep sleep
        repeat: true
        interval: 300 // 5 minutes
        timerWindow: 10
        running: context.sailorgram.connected && (Qt.application.state !== Qt.ApplicationActive)
        onTimeout: dialogs.recheck()
    }
    */

    property SailorGram sailorgram: SailorGram {
        telegram: context.telegram

        onConnectedChanged: {
            if(!context.sailorgram.connected)
                return;

            // Update dialogs
            context.dialogs.recheck();
        }

        onOpenDialogRequested: {
            if(pageStack.depth > 1)
                pageStack.pop(dialogspage, PageStackAction.Immediate);

            var dialog = context.telegram.dialog(peerid);

            if(dialog === context.telegram.nullDialog)
            {
                console.warn("Invalid dialog for peerId = " + peerid);
                return;
            }

            if(dialog.encrypted)
                pageStack.push(Qt.resolvedUrl("../pages/secretdialogs/SecretDialogPage.qml"), { "context": context, "dialog": dialog }, PageStackAction.Immediate);
            else
                pageStack.push(Qt.resolvedUrl("../pages/dialogs/DialogPage.qml"), { "context": context, "dialog": dialog }, PageStackAction.Immediate);

            if(Qt.application.state !== Qt.ApplicationActive)
                mainwindow.activate();
        }
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

        onDocumentStickerRecieved: {
            if(document !== context.currentSticker)
                return

            context.telegram.installStickerSet(set.shortName);
            context.currentSticker = null;
        }

        onIncomingMessage: {
            var elaboratedtext = TextElaborator.elaborateNotify(TelegramHelper.messageContent(msg), sailorgram.emojiPath, Theme.fontSizeSmall);
            var user = context.telegram.user(msg.fromId);

            sailorgram.notify(msg, TelegramHelper.completeName(user), elaboratedtext);
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
