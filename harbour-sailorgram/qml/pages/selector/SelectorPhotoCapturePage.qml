import QtQuick 2.1
import QtQuick.Window 2.1
import QtMultimedia 5.0
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
    onOrientationChanged: {
        switch(orientation) {
            case Orientation.Landscape:
                camera.imageCapture.setMetadata("Orientation", 0);
                break;

            case Orientation.LandscapeInverted:
                camera.imageCapture.setMetadata("Orientation", 180);
                break;

            case Orientation.Portrait:
                camera.imageCapture.setMetadata("Orientation", 270);
                break;

            case Orientation.PortraitInverted:

                camera.imageCapture.setMetadata("Orientation", 90);
                break;

            default:
                camera.imageCapture.setMetadata("Orientation", 0);
                break;
        }
    }

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
                        return 270;
                    case Orientation.LandscapeInverted:
                        return 90;
                    case Orientation.Portrait:
                        return 0;
                    case Orientation.PortraitInverted:
                        return 180;
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
                    case Orientation.Landscape:
                        return 0;
                    case Orientation.LandscapeInverted:
                        return 180;
                    case Orientation.Portrait:
                        return 90;
                    case Orientation.PortraitInverted:
                        return 270;
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
