import QtQuick 2.1
import Sailfish.Silica 1.0

Column
{
    property alias title: titlelabel.text
    property alias copyright: copyrightlabel.text
    property string licenselink: ""
    property string link: ""

    id: thirdpartylabel
    anchors { left: parent.left; right: parent.right }

    Label
    {
        id: titlelabel
        anchors { left: parent.left; right: parent.right }
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
    }

    Label
    {
        id: copyrightlabel
        anchors { left: parent.left; right: parent.right }
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: Theme.fontSizeSmall
        wrapMode: Text.WordWrap
        color: Theme.secondaryColor
    }

    Column
    {
        anchors { left: parent.left; right: parent.right; }
        spacing: Theme.paddingSmall

        Button
        {
            id: licensebutton
            visible: licenselink.length > 0
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("License")
            onClicked: Qt.openUrlExternally(licenselink)
        }

        Button
        {
            id: projectpagebutton
            visible: link.length > 0
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Project Page")
            onClicked: Qt.openUrlExternally(link)
        }
    }
}
