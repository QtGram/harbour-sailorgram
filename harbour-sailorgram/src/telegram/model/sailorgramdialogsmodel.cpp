#include "sailorgramdialogsmodel.h"
#include "../sailorgramtools.h"
#include <telegram/objects/messageobject.h>
#include <telegram/objects/chatobject.h>
#include <telegram/objects/userobject.h>
#include <telegramshareddatamanager.h>

SailorgramDialogsModel::SailorgramDialogsModel(QObject *parent) : SailorgramIdentityProxyModel(parent), _dialoglistmodel(NULL), _writableonly(false)
{

}

bool SailorgramDialogsModel::writableOnly() const
{
    return this->_writableonly;
}

void SailorgramDialogsModel::setWritableOnly(bool b)
{
    if(this->_writableonly == b)
        return;

    this->_writableonly = b;
    this->updateVisibility();
    emit writableOnlyChanged();
}

QVariant SailorgramDialogsModel::data(const QModelIndex &proxyindex, int role) const
{
    if(!this->_dialoglistmodel || (role != SailorgramDialogsModel::RoleItem))
        return QVariant();

    QModelIndex sourceindex = this->mapToSource(proxyindex);
    DialogObject* dialog = SOURCE_ROLE_DATA(DialogObject*, sourceindex, TelegramDialogListModel::RoleDialogItem);
    QByteArray peerkey = TelegramTools::identifier(dialog->core());

    return QVariant::fromValue<SailorgramDialogItem*>(this->_dialogs[peerkey]);
}

QHash<int, QByteArray> SailorgramDialogsModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[SailorgramDialogsModel::RoleItem] = "item";
    return roles;
}

SailorgramDialogItem *SailorgramDialogsModel::dialog(const QByteArray &peerkey)
{
    if(!this->_engine)
        return NULL;

    if(!this->_dialogs.contains(peerkey))
    {
        TelegramSharedDataManager* tsdm = this->_engine->sharedData();
        TelegramSharedPointer<DialogObject> dialog = tsdm->getDialog(peerkey);

        if(!dialog)
            return NULL;

        qint64 accesshash = 0;

        if((dialog->peer()->classType() == PeerObject::TypePeerChannel) || (dialog->peer()->classType() == PeerObject::TypePeerChannel))
        {
            TelegramSharedPointer<ChatObject> chat = tsdm->getChat(peerkey);
            accesshash = chat->accessHash();
        }
        else
        {
            TelegramSharedPointer<UserObject> user = tsdm->getUser(peerkey);
            accesshash = user->accessHash();
        }

        InputPeerObject inputpeer(TelegramTools::peerInputPeer(dialog->peer()->core(), accesshash));
        int idx = this->_dialoglistmodel->indexOf(&inputpeer);

        if(idx == -1)
            return NULL;

        this->inserted(this->_dialoglistmodel->index(idx));
    }

    return this->_dialogs[peerkey];
}

void SailorgramDialogsModel::inserted(QModelIndex sourceindex)
{
    DialogObject* dialog = SOURCE_ROLE_DATA(DialogObject*, sourceindex, TelegramDialogListModel::RoleDialogItem);
    QByteArray peerkey = TelegramTools::identifier(dialog->core());

    if(this->_dialogs.contains(peerkey))
        this->_dialogs[peerkey]->deleteLater();

    SailorgramDialogsModel* thethis = this;
    SailorgramDialogItem* sgdialog = new SailorgramDialogItem(this->_engine, dialog, this);

    connect(sgdialog, &SailorgramDialogItem::unreadCountRequest, [thethis, sourceindex](int count) {
        thethis->sourceModel()->setData(sourceindex, QVariant::fromValue<int>(count), TelegramDialogListModel::RoleUnreadCount);
    });

    connect(sgdialog, &SailorgramDialogItem::muteRequest, [thethis, sourceindex](bool mute) {
        thethis->sourceModel()->setData(sourceindex, QVariant::fromValue<bool>(mute), TelegramDialogListModel::RoleMute);
    });

    connect(sgdialog, &SailorgramDialogItem::messageTextChanged, [thethis, sgdialog, sourceindex]() {
        thethis->updateData(sgdialog, sourceindex, ROLE_LIST(TelegramDialogListModel::RoleUserItem));
    });

    this->updateData(sgdialog, sourceindex, ROLE_LIST(TelegramDialogListModel::RoleName <<
                                                      TelegramDialogListModel::RolePeerItem <<
                                                      TelegramDialogListModel::RoleTopMessageItem <<
                                                      TelegramDialogListModel::RoleMessageUser <<
                                                      TelegramDialogListModel::RoleMessage <<
                                                      TelegramDialogListModel::RoleMessageDate <<
                                                      TelegramDialogListModel::RoleMessageType <<
                                                      TelegramDialogListModel::RoleUnreadCount <<
                                                      TelegramDialogListModel::RoleIsSecretChat <<
                                                      TelegramDialogListModel::RoleMessageOut <<
                                                      TelegramDialogListModel::RoleMessageUnread <<
                                                      TelegramDialogListModel::RoleStatusText <<
                                                      TelegramDialogListModel::RoleMute <<
                                                      TelegramDialogListModel::RoleChatItem <<
                                                      TelegramDialogListModel::RoleUserItem));
    this->_dialogs[peerkey] = sgdialog;
    SailorgramIdentityProxyModel::inserted(sourceindex);
}

void SailorgramDialogsModel::removed(QModelIndex sourceindex)
{
    DialogObject* dialog = SOURCE_ROLE_DATA(DialogObject*, sourceindex, TelegramDialogListModel::RoleDialogItem);
    QByteArray peerkey = TelegramTools::identifier(dialog->core());

    if(this->_dialogs.contains(peerkey))
    {
        SailorgramDialogItem* item = this->_dialogs[peerkey];

        this->_dialogs.remove(peerkey);
        item->deleteLater();
    }

    SailorgramIdentityProxyModel::removed(sourceindex);
}

void SailorgramDialogsModel::changed(QModelIndex sourceindex, const QVector<int> &roles)
{
    DialogObject* dialog = SOURCE_ROLE_DATA(DialogObject*, sourceindex, TelegramDialogListModel::RoleDialogItem);
    QByteArray peerkey = TelegramTools::identifier(dialog->core());

    this->updateData(this->_dialogs[peerkey], sourceindex, roles);
}

void SailorgramDialogsModel::clear()
{
    qDeleteAll(this->_dialogs);
    this->_dialogs.clear();
}

bool SailorgramDialogsModel::contains(QModelIndex sourceindex) const
{
    DialogObject* dialog = SOURCE_ROLE_DATA(DialogObject*, sourceindex, TelegramDialogListModel::RoleDialogItem);
    QByteArray peerkey = TelegramTools::identifier(dialog->core());
    return this->_dialogs.contains(peerkey);
}

void SailorgramDialogsModel::updateData(SailorgramDialogItem *sgdialog, const QModelIndex &sourceindex, const QVector<int> &roles)
{
    foreach(int role, roles)
    {
        if(role == TelegramDialogListModel::RoleName)
            sgdialog->setTitle(SOURCE_ROLE_DATA(QString, sourceindex, role));
        else if(role == TelegramDialogListModel::RolePeerItem)
            sgdialog->setPeer(SOURCE_ROLE_DATA(InputPeerObject*, sourceindex, role));
        else if(role == TelegramDialogListModel::RoleTopMessageItem)
            sgdialog->setTopMessage(SOURCE_ROLE_DATA(MessageObject*, sourceindex, role));
        else if(role == TelegramDialogListModel::RoleMessageUser)
            sgdialog->setMessageUser(SOURCE_ROLE_DATA(UserObject*, sourceindex, role));
        else if(role == TelegramDialogListModel::RoleMessage)
            sgdialog->setMessageText(SOURCE_ROLE_DATA(QString, sourceindex, role));
        else if(role == TelegramDialogListModel::RoleMessageDate)
            sgdialog->setMessageDate(SOURCE_ROLE_DATA(QString, sourceindex, role));
        else if(role == TelegramDialogListModel::RoleMessageType)
            sgdialog->setMessageType(SOURCE_ROLE_DATA(int, sourceindex, role));
        else if(role == TelegramDialogListModel::RoleUnreadCount)
            sgdialog->updateUnreadCount(SOURCE_ROLE_DATA(int, sourceindex, role));
        else if(role == TelegramDialogListModel::RoleIsSecretChat)
            sgdialog->setIsSecretChat(SOURCE_ROLE_DATA(bool, sourceindex, role));
        else if(role == TelegramDialogListModel::RoleMessageOut)
            sgdialog->setIsMessageOut(SOURCE_ROLE_DATA(bool, sourceindex, role));
        else if(role == TelegramDialogListModel::RoleMessageUnread)
            sgdialog->setIsMessageUnread(SOURCE_ROLE_DATA(bool, sourceindex, role));
        else if(role == TelegramDialogListModel::RoleStatusText)
            sgdialog->setStatusText(SOURCE_ROLE_DATA(QString, sourceindex, role));
        else if(role == TelegramDialogListModel::RoleMute)
            sgdialog->updateIsMute(SOURCE_ROLE_DATA(bool, sourceindex, role));
        else if(role == TelegramDialogListModel::RoleChatItem)
            sgdialog->setChat(SOURCE_ROLE_DATA(ChatObject*, sourceindex, role));
        else if(role == TelegramDialogListModel::RoleUserItem)
            sgdialog->setUser(SOURCE_ROLE_DATA(UserObject*, sourceindex, role));
        else if(role == TelegramDialogListModel::RoleTyping)
            sgdialog->setTypingUsers(SOURCE_ROLE_DATA(QVariantList, sourceindex, role));
    }
}

void SailorgramDialogsModel::updateVisibility()
{
    if(!this->_dialoglistmodel)
        return;

    int flags = TelegramDialogListModel::VisibilityUsers |
                TelegramDialogListModel::VisibilityChats |
                TelegramDialogListModel::VisibilitySecretChats |
                TelegramDialogListModel::VisibilityBots;

    if(!this->_writableonly)
        flags |= TelegramDialogListModel::VisibilityChannels;

    this->_dialoglistmodel->setVisibility(flags);
}

void SailorgramDialogsModel::init(TelegramEngine *engine)
{
    if(this->_dialoglistmodel)
        this->_dialoglistmodel->deleteLater();

    this->_dialoglistmodel = new TelegramDialogListModel(this);
    this->updateVisibility();

    this->_dialoglistmodel->setEngine(engine);
    this->bindTo(this->_dialoglistmodel);
}
