import QtQuick 2.1
import Sailfish.Silica 1.0
import "models"
import "pages/login"
import "cover"
import "js/Settings.js" as Settings

ApplicationWindow
{
    default property alias context: context

    Context
    {
        id: context
        sailorgram.onWakeUpRequested: mainwindow.activate()

        Component.onCompleted: {
            Settings.load(function(tx) {
                context.sendwithreturn = parseInt(Settings.transactionGet(tx, "sendwithreturn"));
                context.backgrounddisabled = parseInt(Settings.transactionGet(tx, "backgrounddisabled"));
                context.chatheaderhidden = parseInt(Settings.transactionGet(tx, "chatheaderhidden"));
                context.immediateopen = parseInt(Settings.transactionGet(tx, "immediateopen"));
                context.bubbleshidden = parseInt(Settings.transactionGet(tx, "hidebubbles"));
                context.angledbubbles = parseInt(Settings.transactionGet(tx, "angledbubbles"));
                context.sailorgram.keepRunning = parseInt(Settings.transactionGet(tx, "keeprunning"));

                var opacity = Settings.transactionGet(tx, "bubblesopacity");
                context.bubblesopacity = (opacity === false) ? 100 : parseInt(opacity);
            });
        }
    }

    id: mainwindow

    cover: CoverPage {
        context: mainwindow.context
    }

    initialPage: Component {
        ConnectionPage {
            context: mainwindow.context
        }
    }
}

