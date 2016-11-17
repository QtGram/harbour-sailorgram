import QtQuick 2.1
import QtPositioning 5.0
import Sailfish.Silica 1.0
import harbour.sailorgram.LibQTelegram 1.0
import harbour.sailorgram.SailorGram 1.0
import "../js/Settings.js" as Settings

Item
{
    readonly property int apiId: 27782
    readonly property string apiHash: "5ce096f34c8afab871edce728e6d64c9"
    readonly property string hereAppId: "MqR7KyY6dZpTbKiFwc3h"
    readonly property string hereAppCode: "zfYp6V9Ou_wDQn4NVqMofA"
    readonly property string version: "0.89"
    readonly property bool beta: true
    readonly property int betanum: 1

    readonly property int bubbleradius: {
        if(context.angledbubbles)
            return 20;

        if(!context.bubbleshidden)
            return 4;

        return 0;
    }

    property bool bubbleshidden: false
    property bool angledbubbles: false
    property bool sendwithreturn: false
    property bool chatheaderhidden: false
    property bool immediateopen: false
    property bool autoloadimages:false
    property bool showsearchfield: false
    property bool defaultemojiset: false
    property real bubblesopacity: 100.0

    property Page mainPage: null

    property PositionSource positionSource: PositionSource {
        id: positionsource
        preferredPositioningMethods: PositionSource.AllPositioningMethods
    }

    property Telegram telegram: Telegram {
        initializer: TelegramInitializer {
            debugMode: true
            apiId: context.apiId
            apiHash: context.apiHash
            host: "149.154.167.50"
            port: 443
            dcId: 2
            publicKey: sailorgram.publicKey
        }

        onLoginCompleted: Settings.set("phonenumber", initializer.phoneNumber);
    }

    property SailorGram sailorgram: SailorGram {
        telegram: context.telegram
    }

    property DialogsModel dialogs: DialogsModel {
        telegram: context.telegram
    }

    function versionString() {
        var ver = context.version;

        if(beta)
            ver += " BETA " + betanum;

        return ver + " (LAYER " + context.telegram.apiLayer + ")";
    }

    function locationThumbnail(latitude, longitude, width, height, z) {
        return "https://maps.nlp.nokia.com/mia/1.6/mapview?" + "app_id=" + hereAppId + "&"
                                                             + "app_code=" + hereAppCode + "&"
                                                             + "nord&f=0&poithm=1&poilbl=0&"
                                                             + "ctr=" + latitude + "," + longitude + "&"
                                                             + "w=" + width + "&h=" + height + "&z=" + z;
    }

    id: context

    Component.onCompleted: {
        Settings.load(function(tx) {
            context.sendwithreturn = parseInt(Settings.transactionGet(tx, "sendwithreturn"));
            context.chatheaderhidden = parseInt(Settings.transactionGet(tx, "chatheaderhidden"));
            context.immediateopen = parseInt(Settings.transactionGet(tx, "immediateopen"));
            context.autoloadimages = parseInt(Settings.transactionGet(tx, "autoloadimages"));
            context.bubbleshidden = parseInt(Settings.transactionGet(tx, "hidebubbles"));
            context.angledbubbles = parseInt(Settings.transactionGet(tx, "angledbubbles"));
            context.showsearchfield = parseInt(Settings.transactionGet(tx, "showsearchfield"));
            context.defaultemojiset = parseInt(Settings.transactionGet(tx, "defaultemojiset"));
            context.sailorgram.keepRunning = parseInt(Settings.transactionGet(tx, "keeprunning"));

            var opacity = Settings.transactionGet(tx, "bubblesopacity");
            context.bubblesopacity = (opacity === false) ? 100 : parseInt(opacity);
        });
    }
}
