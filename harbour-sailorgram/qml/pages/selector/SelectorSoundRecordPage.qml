import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.sailorgram.SailorGram 1.0
import harbour.sailorgram.Selector 1.0
import "../../models"

Dialog
{
    property Context context
    property bool started  : false
    property bool finished : false

    signal actionCompleted(string action, var data)

    id: selectorsoundrecordpage
    allowedOrientations: defaultAllowedOrientations
    acceptDestinationAction: PageStackAction.Pop
    canAccept: finished
    onAccepted: actionCompleted("soundrecord", context.sailorgram.voiceRecordPath)

    AudioRecorder { id: recorder }
    DialogHeader { id: header }

    Item {
        anchors {
            fill: parent
            topMargin: header.height
        }

        Column {
            spacing: (Theme.paddingLarge * 2)
            anchors.centerIn: parent

            Label {
                text: recorder.statusMessage
                font.bold: true
                font.family: Theme.fontFamilyHeading
                font.pixelSize: Theme.fontSizeLarge
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Label {
                text: qsTr ("%1 sec").arg ((recorder.duration / 1000).toFixed (3))
                font.family: Theme.fontFamilyHeading
                font.pixelSize: Theme.fontSizeLarge
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Label {
                text: {
                    var bytes = recorder.size
                    if (bytes < 1024) {
                        return qsTr ("~%1 B").arg (bytes.toFixed (0))
                    }
                    else {
                        var kilobytes = (bytes / 1024)
                        if (kilobytes < 1024) {
                            return qsTr ("~%1 KiB").arg (kilobytes.toFixed (1))
                        }
                        else {
                            var megabytes = (kilobytes / 1024)
                            if (megabytes < 1024) {
                                return qsTr ("~%1 MiB").arg (megabytes.toFixed (2))
                            }
                            else {
                                var gigabytes = (megabytes / 1024)
                                return qsTr ("~%1 GiB").arg (gigabytes.toFixed (3))
                            }
                        }
                    }
                }
                font.family: Theme.fontFamilyHeading
                font.pixelSize: Theme.fontSizeSmall
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Rectangle {
                color: Theme.secondaryColor
                width: (selectorsoundrecordpage.width / 2)
                height: 1
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Button {
                text: qsTr ("Start")
                enabled: !started
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    started = true
                    finished = false
                    recorder.start ()
                }
            }
            Button {
                text: qsTr ("Stop")
                enabled: (started && !finished)
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    recorder.stop ()
                    started = false
                    finished = true
                }
            }
        }
    }
}
