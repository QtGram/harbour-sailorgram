import QtQuick 2.1
import Sailfish.Silica 1.0

Page
{
    property string filePath

    signal imageSelected()

    SilicaFlickable
    {
        anchors.fill: parent

        PullDownMenu
        {
            MenuItem
            {
                text: qsTr("Select")
                onClicked: imageSelected()
            }
        }

        Image
        {
            anchors.fill: parent
            source: filePath
            fillMode: Image.PreserveAspectFit
        }
    }
}

