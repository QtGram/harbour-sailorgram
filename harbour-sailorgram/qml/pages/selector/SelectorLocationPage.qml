import QtQuick 2.0
import Sailfish.Silica 1.0
import QtLocation 5.0
import QtPositioning 5.0
import harbour.sailorgram.TelegramQml 2.0 as Telegram
import "../../models"

Dialog
{
    property Context context
    property bool searching : true
    property real latitude  : 0.0
    property real longitude : 0.0

    signal actionCompleted(int actiontype, var data)
    signal actionRequested(int actiontype)
    signal actionRejected()

    id: selectorlocationpage
    allowedOrientations: defaultAllowedOrientations
    acceptDestinationAction: PageStackAction.Pop
    canAccept: !searching

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        actionRequested(Telegram.SendMessageAction.TypeSendMessageGeoLocationAction);
    }

    onAccepted: actionCompleted(SendMessageAction.TypeSendMessageGeoLocationAction, { "latitude":  selectorlocationpage.latitude, "longitude": selectorlocationpage.longitude });
    onRejected: actionRejected();

    DialogHeader { id: header }

    Column
    {
        spacing: Theme.paddingLarge
        anchors.centerIn: parent

        Label {
            text: qsTr("Waiting for coordinates...")
            visible: searching
            font.family: Theme.fontFamilyHeading
            font.pixelSize: Theme.fontSizeExtraLarge
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            text: qsTr("Coordinates")
            visible: !searching
            font.bold: true
            font.family: Theme.fontFamilyHeading
            font.pixelSize: Theme.fontSizeExtraLarge
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            text: qsTr("lat: %1").arg(latitude)
            visible: !searching
            font.family: Theme.fontFamilyHeading
            font.pixelSize: Theme.fontSizeLarge
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            text: qsTr("long: %1").arg(longitude)
            visible: !searching
            font.family: Theme.fontFamilyHeading
            font.pixelSize: Theme.fontSizeLarge
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Loader
        {
            active: !searching

            sourceComponent: Map {
                readonly property int size : Math.min(selectorlocationpage.width, selectorlocationpage.height) * 0.65

                id: map
                clip: true
                width: size
                height: size
                gesture.enabled: false
                center { latitude: selectorlocationpage.latitude; longitude: selectorlocationpage.longitude }
                plugin: Plugin { preferred: ["osm", "here", "nokia", "gmaps"] }

                Timer
                {
                    repeat: true
                    running: true
                    interval: 500
                    triggeredOnStart: true
                    onTriggered: { map.fitViewportToMapItems(); }
                }

                MapCircle
                {
                    id: area
                    radius: 1000; // 1km area
                    opacity: 0
                    center { latitude: selectorlocationpage.latitude; longitude: selectorlocationpage.longitude }
                }

                MapCircle
                {
                    id: point
                    color: Theme.highlightColor
                    radius: 100; // 100m point
                    smooth: true
                    opacity: 0.65
                    antialiasing: true
                    border { width: 2; color: Theme.primaryColor }
                    center { latitude: selectorlocationpage.latitude; longitude: selectorlocationpage.longitude }
                }
            }
        }
    }

    Timer
    {
        property PositionSource positionSource : null

        id: timpossrcrecreation
        repeat: false
        running: true
        interval: 1500
        triggeredOnStart: true

        onTriggered: {
            positionSource = positionsourcecomponent.createObject(selectorlocationpage, { "initialTimestamp" : new Date().getTime() });
        }
    }

    Component
    {
        id: positionsourcecomponent

        PositionSource
        {
            property int initialTimestamp : 0

            id: possrc
            active: true
            updateInterval: 1000

            onPositionChanged: {
                if(position && position.horizontalAccuracyValid && position.horizontalAccuracy > 0 &&
                    position.timestamp.getTime() >= initialTimestamp && position.coordinate && position.coordinate.isValid)
                {
                    latitude = position.coordinate.latitude;
                    longitude = position.coordinate.longitude;
                    searching = false;
                    active = false;
                    destroy();
                }
            }

            onSourceErrorChanged: {
                if(sourceError === PositionSource.ClosedError) {
                    timpossrcrecreation.restart();
                    destroy();
                }
            }
        }
    }
}
