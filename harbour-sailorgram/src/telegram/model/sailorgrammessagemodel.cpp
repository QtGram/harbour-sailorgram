#include "sailorgrammessagemodel.h"
#include <telegram/objects/sendmessageactionobject.h>
#include <telegram/objects/messageobject.h>
#include <telegramstatustyping.h>
#include "../sailorgramtools.h"
#include "../sailorgramenums.h"

#define TYPING_TIMER_INTERVAL 3000

SailorgramMessageModel::SailorgramMessageModel(QObject *parent) : SailorgramIdentityProxyModel(parent), _messagelistmodel(NULL), _currentpeer(NULL)
{
    this->_typingtimer = new QTimer();
    this->_typingtimer->setInterval(TYPING_TIMER_INTERVAL);

    this->_status = new TelegramStatus(this);

    connect(this->_typingtimer, &QTimer::timeout, this, &SailorgramMessageModel::onTypingTimerTimeout);
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

int SailorgramMessageModel::typingStatus() const
{
    return SailorgramTools::sendMessageActionType(this->_status->typing()->action()->classType());
}

void SailorgramMessageModel::setCurrentPeer(InputPeerObject *currentpeer)
{
    if(this->_currentpeer == currentpeer)
        return;

    this->_currentpeer = currentpeer;
    this->_status->typing()->setPeer(currentpeer);

    this->setTypingStatus(SailorgramEnums::TypingStatusCancel);
    this->init(this->_engine);

    emit currentPeerChanged();
}

void SailorgramMessageModel::setLimit(int limit)
{
    if(!this->_messagelistmodel)
        return;

    this->_messagelistmodel->setLimit(limit);
}

void SailorgramMessageModel::setTypingStatus(int typingstatus)
{
    quint32 ts = SailorgramTools::reverseSendMessageActionType(typingstatus);

    if(ts == this->_status->typing()->action()->classType())
        return;

    this->_typingtimer->start();
    this->_status->typing()->action()->setClassType(ts);
    emit typingStatusChanged();
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
                                                       TelegramMessageListModel::RoleToPeerItem <<
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

void SailorgramMessageModel::onTypingTimerTimeout()
{
    this->setTypingStatus(SailorgramEnums::TypingStatusCancel);
}

void SailorgramMessageModel::markAsRead()
{
    if(!this->_messagelistmodel)
        return;

    this->_messagelistmodel->markAsRead();
}

void SailorgramMessageModel::sendMessage(const QString& message, SailorgramMessageItem *replyto)
{
    if(!this->_messagelistmodel)
        return;

    this->_messagelistmodel->sendMessage(message, replyto->rawMessage());
}

void SailorgramMessageModel::sendFile(int type, const QString &file, SailorgramMessageItem *replyto)
{
    if(!this->_messagelistmodel)
        return;

    this->_messagelistmodel->sendFile(SailorgramTools::reverseSendFileType(type), file, replyto->rawMessage());
}

void SailorgramMessageModel::deleteMessages(const QList<SailorgramMessageItem *> messages)
{
    if(!this->_messagelistmodel || messages.isEmpty())
        return;

    QList<qint32> msgids;

    foreach(SailorgramMessageItem* item, messages)
        msgids << item->rawMessage()->id();

    this->_messagelistmodel->deleteMessages(msgids);
}

void SailorgramMessageModel::forwardMessages()
{

}

void SailorgramMessageModel::updateData(SailorgramMessageItem *sgmessage, const QModelIndex &sourceindex, const QVector<int> &roles)
{
    foreach(int role, roles)
    {
        if(role == TelegramMessageListModel::RoleMessageItem)     // Message management v
            sgmessage->setMessage(SOURCE_ROLE_DATA(MessageObject*, sourceindex, role));
        else if(role == TelegramMessageListModel::RoleToPeerItem)
            sgmessage->setFromPeer(SOURCE_ROLE_DATA(InputPeerObject*, sourceindex, role));
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
