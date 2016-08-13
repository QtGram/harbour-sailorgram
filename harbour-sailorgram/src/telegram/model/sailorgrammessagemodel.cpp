#include "sailorgrammessagemodel.h"
#include <telegram/objects/messageobject.h>

SailorgramMessageModel::SailorgramMessageModel(QObject *parent) : SailorgramIdentityProxyModel(parent), _messagelistmodel(NULL), _currentpeer(NULL)
{

}

InputPeerObject *SailorgramMessageModel::currentPeer() const
{
    return this->_currentpeer;
}

int SailorgramMessageModel::limit() const
{
    if(!this->_messagelistmodel)
        return 0;

    return this->_messagelistmodel->limit();
}

void SailorgramMessageModel::setCurrentPeer(InputPeerObject *currentpeer)
{
    if(this->_currentpeer == currentpeer)
        return;

    this->_currentpeer = currentpeer;
    this->init(this->_engine);
    emit currentPeerChanged();
}

void SailorgramMessageModel::setLimit(int limit)
{
    if(!this->_messagelistmodel)
        return;

    this->_messagelistmodel->setLimit(limit);
}

QVariant SailorgramMessageModel::data(const QModelIndex &proxyindex, int role) const
{
    if(!this->_messagelistmodel || (role != SailorgramMessageModel::RoleItem))
        return QVariant();

    QModelIndex sourceindex = this->mapToSource(proxyindex);
    MessageObject* message = SOURCE_ROLE_DATA(MessageObject*, sourceindex, TelegramMessageListModel::RoleMessageItem);

    return QVariant::fromValue<SailorgramMessageItem*>(this->_messages[message->id()]);
}

QHash<int, QByteArray> SailorgramMessageModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[SailorgramMessageModel::RoleItem] = "item";
    return roles;
}

void SailorgramMessageModel::init(TelegramEngine *engine)
{
    if(!engine || !this->_currentpeer)
        return;

    if(this->_messagelistmodel)
        this->_messagelistmodel->deleteLater();

    this->_messagelistmodel = new TelegramMessageListModel(this);
    connect(this->_messagelistmodel, &TelegramMessageListModel::limitChanged, this, &SailorgramMessageModel::limitChanged);

    this->_messagelistmodel->setEngine(engine);
    this->_messagelistmodel->setCurrentPeer(this->_currentpeer);
    this->bindTo(this->_messagelistmodel);
}

void SailorgramMessageModel::inserted(QModelIndex sourceindex)
{
    MessageObject* message = SOURCE_ROLE_DATA(MessageObject*, sourceindex, TelegramMessageListModel::RoleMessageItem);

    if(this->_messages.contains(message->id()))
        this->_messages[message->id()]->deleteLater();

    SailorgramMessageModel* thethis = this;
    SailorgramMessageItem* sgmessage = new SailorgramMessageItem(this->_engine, this);

    connect(sgmessage->messageMedia(), &SailorgramMessageMedia::downloadRequest, [thethis, sourceindex](bool start) {
        thethis->_messagelistmodel->setData(sourceindex, QVariant::fromValue<bool>(start), TelegramMessageListModel::RoleDownloading);
    });

    this->updateData(sgmessage, sourceindex, ROLE_LIST(TelegramMessageListModel::RoleMessageItem <<   // Message Management v
                                                       TelegramMessageListModel::RoleFromUserItem <<
                                                       TelegramMessageListModel::RoleToUserItem <<
                                                       TelegramMessageListModel::RoleMessage <<
                                                       TelegramMessageListModel::RoleDateTime <<
                                                       TelegramMessageListModel::RoleMessageType <<
                                                       TelegramMessageListModel::RoleUnread <<
                                                       TelegramMessageListModel::RoleOut <<
                                                       TelegramMessageListModel::RoleForwardFromPeer <<
                                                       TelegramMessageListModel::RoleReplyPeer <<
                                                       TelegramMessageListModel::RoleReplyMessage <<  // Reply Management v
                                                       TelegramMessageListModel::RoleReplyType <<
                                                       TelegramMessageListModel::RoleMediaItem <<     // Media management v
                                                       TelegramMessageListModel::RoleFileName <<
                                                       TelegramMessageListModel::RoleFilePath <<
                                                       TelegramMessageListModel::RoleFileMimeType <<
                                                       TelegramMessageListModel::RoleFileSize <<
                                                       TelegramMessageListModel::RoleThumbPath <<
                                                       TelegramMessageListModel::RoleFileTitle <<
                                                       TelegramMessageListModel::RoleFilePerformer <<
                                                       TelegramMessageListModel::RoleFileDuration <<
                                                       TelegramMessageListModel::RoleFileIsVoice <<
                                                       TelegramMessageListModel::RoleDownloadable <<  // Transfer management v
                                                       TelegramMessageListModel::RoleTransfared <<
                                                       TelegramMessageListModel::RoleTransfaring <<
                                                       TelegramMessageListModel::RoleTransfaredSize));

    this->_messages[message->id()] = sgmessage;
    SailorgramIdentityProxyModel::inserted(sourceindex);
}

void SailorgramMessageModel::removed(QModelIndex sourceindex)
{
    MessageObject* message = SOURCE_ROLE_DATA(MessageObject*, sourceindex, TelegramMessageListModel::RoleMessageItem);

    if(this->_messages.contains(message->id()))
    {
        SailorgramMessageItem* item = this->_messages[message->id()];

        this->_messages.remove(message->id());
        item->deleteLater();
    }

    SailorgramIdentityProxyModel::removed(sourceindex);
}

void SailorgramMessageModel::changed(QModelIndex sourceindex, const QVector<int> &roles)
{
    MessageObject* message = SOURCE_ROLE_DATA(MessageObject*, sourceindex, TelegramMessageListModel::RoleMessageItem);
    this->updateData(this->_messages[message->id()], sourceindex, roles);
}

void SailorgramMessageModel::clear()
{
    qDeleteAll(this->_messages);
    this->_messages.clear();
}

bool SailorgramMessageModel::contains(QModelIndex sourceindex) const
{
    MessageObject* message = SOURCE_ROLE_DATA(MessageObject*, sourceindex, TelegramMessageListModel::RoleMessageItem);
    return this->_messages.contains(message->id());
}

void SailorgramMessageModel::markAsRead()
{
    if(!this->_messagelistmodel)
        return;

    this->_messagelistmodel->markAsRead();
}

void SailorgramMessageModel::updateData(SailorgramMessageItem *sgmessage, const QModelIndex &sourceindex, const QVector<int> &roles)
{
    foreach(int role, roles)
    {
        if(role == TelegramMessageListModel::RoleMessageItem)     // Message management v
            sgmessage->setMessage(SOURCE_ROLE_DATA(MessageObject*, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleFromUserItem)
            sgmessage->setFromUser(SOURCE_ROLE_DATA(UserObject*, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleToUserItem)
            sgmessage->setToUser(SOURCE_ROLE_DATA(UserObject*, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleMessage)
            sgmessage->setMessageText(SOURCE_ROLE_DATA(QString, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleDateTime)
            sgmessage->setMessageDate(SOURCE_ROLE_DATA(QString, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleMessageType)
            sgmessage->setMessageType(SOURCE_ROLE_DATA(int, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleUnread)
            sgmessage->setIsMessageUnread(SOURCE_ROLE_DATA(bool, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleOut)
            sgmessage->setIsMessageOut(SOURCE_ROLE_DATA(bool, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleForwardFromPeer)
            sgmessage->setForwardPeer(new SailorgramPeer(SOURCE_ROLE_DATA_VARIANT(sourceindex, role)));
        else if(role == TelegramMessageListModel::RoleReplyPeer)
            sgmessage->setReplyPeer(new SailorgramPeer(SOURCE_ROLE_DATA_VARIANT(sourceindex, role)));
        else if(role == TelegramMessageListModel::RoleReplyMessage) // Reply management v
            sgmessage->messageReply()->setMessage(SOURCE_ROLE_DATA(MessageObject*, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleReplyType)
            sgmessage->messageReply()->setMessageType(SOURCE_ROLE_DATA(int, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleMediaItem)    // Media management v
            sgmessage->messageMedia()->setMediaObject(SOURCE_ROLE_DATA(MessageMediaObject*, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleFileName)
            sgmessage->messageMedia()->setFileName(SOURCE_ROLE_DATA(QString, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleFilePath)
            sgmessage->messageMedia()->setFilePath(SOURCE_ROLE_DATA(QString, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleFileMimeType)
            sgmessage->messageMedia()->setFileMimeType(SOURCE_ROLE_DATA(QString, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleFileSize)
            sgmessage->messageMedia()->setFileSize(SOURCE_ROLE_DATA(qint32, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleThumbPath)
            sgmessage->messageMedia()->setThumbPath(SOURCE_ROLE_DATA(QString, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleFileTitle)
            sgmessage->messageMedia()->setFileTitle(SOURCE_ROLE_DATA(QString, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleFilePerformer)
            sgmessage->messageMedia()->setFilePerformer(SOURCE_ROLE_DATA(QString, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleFileDuration)
            sgmessage->messageMedia()->setFileDuration(SOURCE_ROLE_DATA(qint32, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleFileIsVoice)
            sgmessage->messageMedia()->setIsVoiceFile(SOURCE_ROLE_DATA(bool, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleDownloadable) // Transfer management v
            sgmessage->messageMedia()->setIsDownloadable(SOURCE_ROLE_DATA(bool, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleTransfared)
            sgmessage->messageMedia()->setIsTransfered(SOURCE_ROLE_DATA(bool, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleTransfaring)
            sgmessage->messageMedia()->setIsTransfering(SOURCE_ROLE_DATA(bool, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleTransfaredSize)
            sgmessage->messageMedia()->setTransferedSize(SOURCE_ROLE_DATA(qint32, sourceindex, role));
    }
}
