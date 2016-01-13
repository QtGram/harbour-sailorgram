import QtQuick 2.1
import QtQuick.Window 2.1
import QtMultimedia 5.4
import Sailfish.Silica 1.0

Dialog
{
    property int capturedOrientation : -1
    property string capturedFilePath    : ""

    signal actionCompleted(string action, var data)

    id: selectorphotopage
    allowedOrientations: defaultAllowedOrientations
    canAccept:(capturedFilePath !== "")
    onAccepted: actionCompleted("photocapture", capturedFilePath)

    DialogHeader { id: header }

    Item
    {
        anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom
                  leftMargin: Theme.paddingMedium; rightMargin: Theme.paddingMedium; bottomMargin: Theme.paddingMedium }

        Camera
        {
            id: camera
            captureMode: Camera.CaptureStillImage
            flash { mode: Camera.FlashAuto }
            focus { focusMode: Camera.FocusContinuous; focusPointMode: Camera.FocusPointCenter }

            imageCapture {
                onImageCaptured: {
                    capturedFilePath = preview
                }
                onImageSaved: {
                    capturedFilePath = path
                }
                onCaptureFailed: {
                    console.error("failed", message)
                }
            }
        }

        VideoOutput
        {
            id: videooutput
            source: camera
            visible:(capturedFilePath === "")
            fillMode: VideoOutput.PreserveAspectFit
            width: (rotation % 180 === 0 ? parent.width : parent.height)
            height: (rotation % 180 === 0 ? parent.height : parent.width)
            anchors.centerIn: parent

            rotation: {
                switch(selectorphotopage.orientation) {
                    case Orientation.Landscape:
                    case Orientation.LandscapeInverted:
                        return -90;

                    case Orientation.Portrait:
                    case Orientation.PortraitInverted:
                    default:
                        return 0;
                }
            }

            MouseArea
            {
                anchors.fill: parent
                onReleased: camera.imageCapture.capture();

                onPressed: {
                    capturedOrientation = selectorphotopage.orientation;
                    camera.searchAndLock();
                }
            }
        }

        Image
        {
            id: imgphotopreview
            source: capturedFilePath
            visible: (capturedFilePath !== "")
            fillMode: Image.PreserveAspectFit
            width: (rotation % 180 === 0 ? parent.width : parent.height)
            height: (rotation % 180 === 0 ? parent.height : parent.width)
            anchors.centerIn: parent

            rotation: {
                switch(capturedOrientation) {
                    case Orientation.Portrait:
                    case Orientation.PortraitInverted:
                        return +90;

                    case Orientation.Landscape:
                    case Orientation.LandscapeInverted:
                    default:
                        return 0;
                }
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    capturedFilePath = "";
                    capturedOrientation = -1;
                }
            }
        }
    }
}
