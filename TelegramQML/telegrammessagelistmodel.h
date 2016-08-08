#ifndef TELEGRAMMESSAGELISTMODEL_H
#define TELEGRAMMESSAGELISTMODEL_H

#include "telegramqml_macros.h"
#include "telegramqml_global.h"
#include "telegramabstractenginelistmodel.h"

#include <QJSValue>
#include <telegram.h>

class ReplyMarkupObject;
class MessageObject;
class DialogObject;
class DocumentObject;
class TelegramMessageListItem;
class InputPeerObject;
class TelegramMessageListModelPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramMessageListModel : public TelegramAbstractEngineListModel
{
    Q_OBJECT
    Q_ENUMS(DataRoles)
    Q_ENUMS(MessageType)
    Q_PROPERTY(bool refreshing READ refreshing NOTIFY refreshingChanged)
    Q_PROPERTY(InputPeerObject* currentPeer READ currentPeer WRITE setCurrentPeer NOTIFY currentPeerChanged)
    Q_PROPERTY(QList<qint32> messageList READ messageList WRITE setMessageList NOTIFY messageListChanged)
    Q_PROPERTY(QJSValue dateConvertorMethod READ dateConvertorMethod WRITE setDateConvertorMethod NOTIFY dateConvertorMethodChanged)
    Q_PROPERTY(QByteArray key READ key NOTIFY keyChanged)
    Q_PROPERTY(QVariantList typingUsers READ typingUsers NOTIFY typingUsersChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(bool megagroup READ megagroup NOTIFY megagroupChanged)
    Q_PROPERTY(bool editable READ editable NOTIFY editableChanged)
    Q_PROPERTY(bool useCache READ useCache WRITE setUseCache NOTIFY useCacheChanged)

public:
    TelegramMessageListModel(QObject *parent = 0);
    ~TelegramMessageListModel();

    enum DataRoles {
        RoleMessageItem = Qt::UserRole,
        RoleMediaItem,
        RoleServiceItem,
        RoleMarkupItem,
        RoleEntityList,
        RoleServiceUserItem,
        RoleFromUserItem,
        RoleToUserItem,
        RoleToChatItem,
        RoleToPeerItem,

        RoleMessage,
        RoleDateTime,
        RoleDate,
        RoleUnread,
        RoleSent,
        RoleOut,
        RoleReplyMsgId,
        RoleReplyMessage,
        RoleReplyPeer,
        RoleForwardFromPeer,
        RoleForwardDate,
        RoleMessageType,
        RoleReplyType,

        RoleFileName,
        RoleFileMimeType,
        RoleFileTitle,
        RoleFilePerformer,
        RoleFileDuration,
        RoleFileIsVoice,
        RoleFileSize,

        RoleDownloadable,
        RoleUploading,
        RoleDownloading,
        RoleTransfaring,
        RoleTransfared,
        RoleTransfaredSize,
        RoleTotalSize,
        RoleFilePath,
        RoleThumbPath
    };

    bool refreshing() const;

    QByteArray id(const QModelIndex &index) const;
    int count() const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QHash<int, QByteArray> roleNames() const;

    virtual void setCurrentPeer(InputPeerObject *dialog);
    virtual InputPeerObject *currentPeer() const;

    /*! Instead of the currentPeer */
    void setMessageList(const QList<qint32> &list);
    QList<qint32> messageList() const;

    QJSValue dateConvertorMethod() const;
    void setDateConvertorMethod(const QJSValue &method);

    void setUseCache(bool useCache);
    bool useCache() const;

    bool megagroup() const;
    bool editable() const;

    int limit() const;
    void setLimit(int limit);

    QByteArray key() const;
    QString statusText() const;
    QVariantList typingUsers() const;
    static QStringList requiredProperties();

Q_SIGNALS:
    void refreshingChanged();
    void currentPeerChanged();
    void messageListChanged();
    void keyChanged();
    void dateConvertorMethodChanged();
    void limitChanged();
    void typingUsersChanged();
    void statusTextChanged();
    void megagroupChanged();
    void editableChanged();
    void useCacheChanged();

public Q_SLOTS:
    bool sendMessage(const QString &message, MessageObject *replyTo = 0, ReplyMarkupObject *replyMarkup = 0, const QJSValue &callback = QJSValue());
    bool sendFile(int type, const QString &file, MessageObject *replyTo = 0, ReplyMarkupObject *replyMarkup = 0, const QJSValue &callback = QJSValue());
    void deleteMessages(const QList<qint32> &msgs, const QJSValue &callback = QJSValue());
    void forwardMessages(InputPeerObject *fromInputPeer, const QList<qint32> &msgs, const QJSValue &callback = QJSValue());
    void resendMessage(qint32 msgId, const QString &newCaption = QString(), const QJSValue &callback = QJSValue());
    void sendSticker(DocumentObject *doc, MessageObject *replyTo = 0, ReplyMarkupObject *replyMarkup = 0, const QJSValue &callback = QJSValue());
    void markAsRead(const QJSValue &callback = QJSValue());
    void clearHistory(bool justClear, const QJSValue &callback = QJSValue());

    virtual void loadFrom(qint32 msgId);
    virtual void loadBack();
    virtual void loadFront();

protected:
    virtual void refresh();
    void clean();
    void resort();
    void setRefreshing(bool stt);
    QByteArray identifier() const;

    void setHasBackMore(bool stt);
    void processOnResult(const MessagesMessages &res, bool appened = false);

    virtual QString convertDate(const QDateTime &td) const;

    void timerEvent(QTimerEvent *e);
    virtual void connectTelegram();

private:
    void getMessagesFromServer(int offsetId, int addOffset, int limit);
    void getMessageListFromServer();
    void fetchReplies(QList<Message> messages, int delay = 100);
    void processOnResult(const class MessagesMessages &result, QHash<QByteArray, TelegramMessageListItem> *items);
    void changed(QHash<QByteArray, TelegramMessageListItem> hash);
    QList<QByteArray> getSortedList(const QHash<QByteArray, TelegramMessageListItem> &items);

    void connectMessageSignals(const QByteArray &id, class MessageObject *message);
    void connectChatSignals(const QByteArray &id, class ChatObject *chat);
    void connectUserSignals(const QByteArray &id, class UserObject *user);
    void connectDialogSignals(const QByteArray &id, class DialogObject *dialog);
    void connectUploaderSignals(const QByteArray &id, class TelegramUploadHandler *handler);
    void connectDownloaderSignals(const QByteArray &id, class TelegramDownloadHandler *downloader);

    virtual void onUpdates(const UpdatesType &update);

    void insertUpdate(const Update &update);

    void clearHistoryAnswer(qint64 msgId, const MessagesAffectedHistory &result);

private:
    TelegramMessageListModelPrivate *p;
};

#endif // TELEGRAMMESSAGELISTMODEL_H
