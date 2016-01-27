/*
  Copyright (C) 2013 Jolla Ltd.
  Contact: Thomas Perl <thomas.perl@jollamobile.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import QtQuick 2.1
import Sailfish.Silica 1.0
import harbour.sailorgram.TelegramQml 1.0
import "../../models"
import "../../components"
import "../../components/search"
import "../../menus/dialog"
import "../../items/dialog"
import "../../js/TelegramHelper.js" as TelegramHelper
import "../../js/TelegramConstants.js" as TelegramConstants

Page
{
    property Context context
    property Component conversationItemComponent
    property Component secretConversationItemComponent

    function displayDialog(dialog) {
        if(dialog.encrypted) {
            pageStack.push(Qt.resolvedUrl("../secretdialogs/SecretDialogPage.qml"), { "context": dialogspage.context, "dialog": dialog });
            return;
        }

        pageStack.push(Qt.resolvedUrl("DialogPage.qml"), { "context": dialogspage.context, "dialog": dialog })
    }

    id: dialogspage
    allowedOrientations: defaultAllowedOrientations

    onStatusChanged: {
        if(dialogspage.status !== PageStatus.Active)
            return;

        pageStack.pushAttached(Qt.resolvedUrl("../contacts/ContactsPage.qml"), { "context": dialogspage.context });
        context.sailorgram.foregroundDialog = null; // Reset Foreground Dialog
        context.dialogs.telegram = context.telegram; // Balance load: Load after DialogsPage is displayed
        context.contacts.telegram = context.telegram;
    }

    Connections
    {
        target: context.sailorgram

        onOpenDialogRequested: {
            if(pageStack.depth > 1)
                pageStack.pop(dialogspage, PageStackAction.Immediate);

            var dialog = context.telegram.dialog(peerid);

            if(dialog === context.telegram.nullDialog)
            {
                console.warn("Invalid dialog for peerId = " + peerid);
                return;
            }

            if(dialog.encrypted)
                pageStack.push(Qt.resolvedUrl("../secretdialogs/SecretDialogPage.qml"), { "context": context, "dialog": dialog }, PageStackAction.Immediate);
            else
                pageStack.push(Qt.resolvedUrl("DialogPage.qml"), { "context": context, "dialog": dialog }, PageStackAction.Immediate);

            if(Qt.application.state !== Qt.ApplicationActive)
                mainwindow.activate();
        }
    }

    SilicaFlickable
    {
        anchors.fill: parent

        DialogsPullDownMenu
        {
            id: dialogsmenu
            context: dialogspage.context
        }

        PageHeader
        {
            id: pageheader
            title: context.sailorgram.connected ? qsTr("Chats") : qsTr("Connecting...")

            ConnectionStatus {
                context: dialogspage.context
                anchors { verticalCenter: pageheader.extraContent.verticalCenter; left: pageheader.extraContent.left; leftMargin: -Theme.horizontalPageMargin }
            }
        }

        SilicaListView
        {
            ViewPlaceholder
            {
                enabled: lvdialogs.count <= 0
                text: qsTr("No Chats\n\nDo a swype to the right to select a contact")
            }

            id: lvdialogs
            spacing: Theme.paddingMedium
            clip: true
            model: context.dialogs
            anchors { left: parent.left; top: pageheader.bottom; right: parent.right; bottom: parent.bottom }

            header: SearchList {
                width: parent.width
                context: dialogspage.context

                onMessageClicked: {
                    var dialog = context.telegram.messageDialog(message.id);
                    displayDialog(dialog);
                }
            }

            delegate: ListItem {
                id: dialogitem
                contentWidth: parent.width
                contentHeight: Theme.itemSizeSmall
                onClicked: displayDialog(item)

                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Delete")

                        onClicked: {
                            var msg = qsTr("Deleting Conversation");

                            if(item.encrypted)
                                msg = qsTr("Deleting Secret Chat");
                            else if(TelegramHelper.isChat(item))
                                msg = qsTr("Deleting Group");

                            dialogitem.remorseAction(msg, function() {
                                item.unreadCount = 0;

                                if(item.encrypted)
                                    context.telegram.messagesDiscardEncryptedChat(item.peer.userId, true);
                                else
                                    context.telegram.messagesDeleteHistory(TelegramHelper.peerId(item), true, false, true);
                            });
                        }
                    }
                }

                Component.onCompleted: {
                    if(!item.encrypted && !conversationItemComponent) {
                        conversationItemComponent = Qt.createComponent("../../items/dialog/DialogItem.qml");

                        if(conversationItemComponent.status === Component.Error) {
                            console.log(conversationItemComponent.errorString());
                            return;
                        }
                    }
                    else if(item.encrypted && !secretConversationItemComponent) {
                        secretConversationItemComponent = Qt.createComponent("../../items/secretdialog/SecretDialogItem.qml");

                        if(secretConversationItemComponent.status === Component.Error) {
                            console.log(secretConversationItemComponent.errorString());
                            return;
                        }
                    }

                    var c = !item.encrypted ? conversationItemComponent : secretConversationItemComponent;
                    c.createObject(contentItem, {"anchors.fill": contentItem, "context": dialogspage.context, "dialog": item });
                }
            }
        }
    }
}
