import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 2.0
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

        context.contacts.engine = context.engine
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

        section {
            property: "firstLetter"
            criteria: ViewSection.FirstCharacter

            delegate: SectionHeader {
                text: section
                font.pixelSize: Theme.fontSizeMedium
            }
        }

        delegate: ListItem {
            contentWidth: parent.width
            contentHeight: Theme.itemSizeSmall

            menu: UserMenu {
                id: usermenu
                context: contactspage.context
                dialogModelItem: model
            }

            onClicked: {
                pageStack.navigateBack(PageStackAction.Immediate);

                pageStack.push(Qt.resolvedUrl("../dialogs/DialogPage.qml"), { "context": dialogspage.context,
                                                                              "title": model.title,
                                                                              "peerHex": model.peerHex,
                                                                              "peer": model.peer,
                                                                              "chat": model.chat,
                                                                              "user": model.user,
                                                                              "secretChatState": model.secretChatState,
                                                                              "isSecretChat": model.isSecretChat })
            }

            UserItem {
                id: useritem
                anchors.fill: parent
                context: contactspage.context
                user: model.user
                statusText: model.statusText
            }
        }
    }
}
