import QtQuick 2.2
import Sailfish.Silica 1.0
import "../../models"

Dialog
{
    property Context context
    property string firstname
    property string lastname
    property string telephonenumber
    id: addcontactdlg

    Component.onCompleted: {
        tf_nameid.text = firstname;
        tf_lastnameid.text = lastname;
        tf_telephoneid.text = telephonenumber;
    }

    onAccepted:
    {
        firstname = tf_nameid.text
        lastname = tf_lastnameid.text
        telephonenumber = tf_telephoneid.text
        context.telegram.addContact(firstname,lastname,telephonenumber)
    }

    SilicaFlickable
    {
        anchors.fill: parent
        DialogHeader
        {
            id: dlgheader
            title: qsTr("Add Contact")
        }
        TextField
        {
            id: tf_nameid
            anchors { left: parent.left; top: dlgheader.bottom; right: parent.right }
            placeholderText: qsTr("First Name")
            label: qsTr("First Name")
        }
        TextField
        {
            id: tf_lastnameid
            anchors { left: parent.left; top: tf_nameid.bottom; right: parent.right }
            placeholderText: qsTr("Last Name")
            label: qsTr("Last Name")
        }
        TextField
        {
            id: tf_telephoneid
            anchors { left: parent.left; top: tf_lastnameid.bottom; right: parent.right }
            inputMethodHints: Qt.ImhDialableCharactersOnly
            placeholderText: qsTr("Telephone Number")
            label: qsTr("Telephone Number")
        }
    }
}

