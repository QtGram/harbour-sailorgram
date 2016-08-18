import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.DBus 1.0
import harbour.sailorgram.SailorGram 1.0
import harbour.sailorgram.Model 1.0
import harbour.sailorgram.Telegram 1.0
import harbour.sailorgram.TelegramQml 2.0
import "../js/Settings.js" as Settings
import "../js/TelegramHelper.js" as TelegramHelper
import "../js/TelegramAction.js" as TelegramAction
import "../js/TextElaborator.js" as TextElaborator

QtObject
{
    id: context

    readonly property string hereAppId: "MqR7KyY6dZpTbKiFwc3h"
    readonly property string hereAppCode: "zfYp6V9Ou_wDQn4NVqMofA"
    readonly property string version: "0.80"
    readonly property bool beta: false
    readonly property int betanum: 1
    readonly property int stepcount: 25

    readonly property int bubbleradius: {
        if(context.angledbubbles)
            return 20;

        if(!context.bubbleshidden)
            return 4;

        return 0;
    }

    property bool sendwithreturn: false
    property bool backgrounddisabled: false
    property bool chatheaderhidden: false
    property bool bubbleshidden: false
    property bool angledbubbles: false
    property bool immediateopen: false
    property bool autoloadimages:false
    property bool showsearchfield: false
    property int bubblesopacity: 100

    function versionString() {
        var ver = context.version;

        if(beta)
            ver += " BETA " + betanum;

        return ver;
    }

    function locationThumbnail(latitude, longitude, width, height, z) {
        return "https://maps.nlp.nokia.com/mia/1.6/mapview?" + "app_id=" + hereAppId + "&"
                                                             + "app_code=" + hereAppCode + "&"
                                                             + "nord&f=0&poithm=1&poilbl=0&"
                                                             + "ctr=" + latitude + "," + longitude + "&"
                                                             + "w=" + width + "&h=" + height + "&z=" + z;
    }

    property Page mainPage: null
    property Document currentSticker: null
    property ScreenBlank screenblank: ScreenBlank { }
    property ErrorsModel errors: ErrorsModel { }
    property SailorgramContactsModel contacts: SailorgramContactsModel { }

    property SailorgramDialogsModel dialogs: SailorgramDialogsModel {
        engine: context.engine
    }

    property SailorgramNotifications notifications: SailorgramNotifications {
        engine: context.engine
        dialogs: context.dialogs
    }

    property StickersCategoriesModel stickers: StickersCategoriesModel {
        engine: context.engine
    }

    property SailorGram sailorgram: SailorGram {
        engine: context.engine
    }

    property Engine engine: Engine {
        configDirectory: sailorgram.configPath
        logLevel: Engine.LogLevelClean

        app: App {
            appId: 27782
            appHash: "5ce096f34c8afab871edce728e6d64c9"
        }

        host: Host {
            hostAddress: "149.154.167.50"
            hostPort: 443
            hostDcId: 2
            publicKey: sailorgram.publicKey
        }

        onPhoneNumberChanged: {
            var phonenumber = Settings.get("phonenumber");

            if(phonenumber !== false)
                return;

            Settings.set("phonenumber", context.engine.phoneNumber); // Save Phone Number for fast login
        }
    }

    property Authenticate authenticate: Authenticate {
        engine: context.engine

        onStateChanged: {
            pageStack.completeAnimation();

            if(state === Authenticate.AuthCodeRequested)
                pageStack.replace(Qt.resolvedUrl("../pages/login/AuthorizationPage.qml"), { "context": context /* FIXME: , "authError": context.telegram.authSignInError */ });
            else if(state === Authenticate.AuthSignUpNeeded)
                pageStack.replace(Qt.resolvedUrl("../pages/login/SignUpPage.qml"), { "context": context /* FIXME: , "authError": context.telegram.authSignUpError */ });
            else if(state === Authenticate.AuthLoggedIn)
                pageStack.replace(Qt.resolvedUrl("../pages/dialogs/DialogsPage.qml"), { "context": context });
            else if(state === Authenticate.AuthSignUpNeeded)
                pageStack.replace(Qt.resolvedUrl("../pages/login/SignUpPage.qml"), { "context": context /* FIXME: , "authError": context.telegram.authSignUpError */ });
            else if(state === Authenticate.AuthCheckingPhoneError)
                console.log("NOT IMPLEMENTED: Authenticate.AuthCheckingPhoneError")
            else if(state === Authenticate.AuthCodeRequestingError)
                pageStack.replace(Qt.resolvedUrl("../pages/login/AuthorizationPage.qml"), { "context": context, "authError": "ERROR" /* FIXME: Better Error Message */ });
            else
                console.log("Unhandled State: " + state);
        }
    }
}
