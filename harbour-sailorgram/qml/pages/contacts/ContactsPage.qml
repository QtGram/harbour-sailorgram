import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../items/user"
import "../../menus"

Page
{
    property Context context

    id: contactspage
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if(status !== PageStatus.Active)
            return;

        context.contacts.telegram = context.telegram;
    }

    SilicaListView
    {
        PullDownMenu
        {
            id: userspulldownmenu

            MenuItem
            {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("../settings/SettingsPage.qml"), { "context": contactspage.context })
            }

            MenuItem
            {
                text: qsTr("Profile")
                onClicked: pageStack.push(Qt.resolvedUrl("../profile/ProfilePage.qml"), { "context": contactspage.context })
            }

            MenuItem
            {
                text: qsTr("Add Contact")
                onClicked: pageStack.push(Qt.resolvedUrl("AddContactPage.qml"), { "context": contactspage.context });
            }
        }

        id: lvcontacts
        anchors.fill: parent
        spacing: Theme.paddingMedium
        header: PageHeader { title: qsTr("Contacts") }
        model: context.contacts

        section.property: "firstLetter"
        section.criteria: ViewSection.FirstCharacter

        section.delegate: SectionHeader {
            text: section
            font.pixelSize: Theme.fontSizeMedium
        }

        delegate: ListItem {
            contentWidth: parent.width
            contentHeight: Theme.itemSizeSmall

            menu: UserMenu {
                id: usermenu
                context: contactspage.context
                user: model.user
            }

            onClicked: pageStack.push(Qt.resolvedUrl("../dialogs/DialogPage.qml"), { "context": contactspage.context, "dialog": context.telegram.fakeDialogObject(model.contact.userId, false) } )

            UserItem {
                id: useritem
                anchors.fill: parent
                context: contactspage.context
                user: model.user
            }
        }
    }
}
