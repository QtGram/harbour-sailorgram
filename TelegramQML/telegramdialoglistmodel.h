#ifndef TELEGRAMDIALOGLISTMODEL_H
#define TELEGRAMDIALOGLISTMODEL_H

#include <QObject>
#include <QJSValue>

#include "telegramqml_macros.h"
#include "telegramqml_global.h"
#include "telegramabstractenginelistmodel.h"

class UpdatesType;
class InputPeerObject;
class TelegramDialogListItem;
class TelegramDialogListModelPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramDialogListModel : public TelegramAbstractEngineListModel
{
    Q_OBJECT
    Q_ENUMS(VisibilityFlags)
    Q_ENUMS(SortFlag)
    Q_ENUMS(DataRoles)
    Q_ENUMS(SecretChatState)
    Q_PROPERTY(int visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
    Q_PROPERTY(QList<qint32> sortFlag READ sortFlag WRITE setSortFlag NOTIFY sortFlagChanged)
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QJSValue dateConvertorMethod READ dateConvertorMethod WRITE setDateConvertorMethod NOTIFY dateConvertorMethodChanged)
    Q_PROPERTY(QJSValue messageTextMethod READ messageTextMethod WRITE setMessageTextMethod NOTIFY messageTextMethodChanged)
    Q_PROPERTY(bool refreshing READ refreshing NOTIFY refreshingChanged)
    Q_PROPERTY(bool useCache READ useCache WRITE setUseCache NOTIFY useCacheChanged)
    Q_PROPERTY(QVariantMap categories READ categories WRITE setCategories NOTIFY categoriesChanged)

public:
    enum VisibilityFlags {
        VisibilityEmptyDialogs = 1,
        VisibilityDeletedDialogs = 2,
        VisibilityOnlineUsersOnly = 4,
        VisibilityContacts = 8,
        VisibilityNonContacts = 16,
        VisibilityBots = 32,
        VisibilityUsers = VisibilityContacts | VisibilityNonContacts,
        VisibilityChats = 64,
        VisibilityChannels = 128,
        VisibilitySecretChats = 256,
        VisibilityAll = VisibilityEmptyDialogs | VisibilityDeletedDialogs | VisibilitySecretChats |
                        VisibilityUsers | VisibilityChats | VisibilityChannels | VisibilityBots
    };

    enum SortFlag {
        SortByType = 0,
        SortByName = 1,
        SortByDate = 2,
        SortByUnreads = 3,
        SortByOnline = 4,
        SortByCategories = 5
    };

    enum DataRoles {
        RoleDialogItem = Qt::UserRole,
        RoleChatItem,
        RoleUserItem,
        RoleTopMessageItem,
        RolePeerItem,
        RolePeerHex,

        RoleIsSecretChat,
        RoleSecretChatState,

        RoleName,
        RoleMessageDate,
        RoleMessageUnread,
        RoleMessage,
        RoleMessageOut,
        RoleMessageType,
        RoleMessageUser,
        RoleLastOnline,
        RoleIsOnline,
        RoleStatus,
        RoleStatusText,
        RoleTyping,
        RoleUnreadCount,
        RoleMute,
        RoleCategory
    };

    enum SecretChatState {
        SecretChatStateInit = 0,
        SecretChatStateRequested,
        SecretChatStateAccepted,
        SecretChatStateCancelled
    };

    TelegramDialogListModel(QObject *parent = 0);
    ~TelegramDialogListModel();

    int visibility() const;
    void setVisibility(int visibility);

    QList<qint32> sortFlag() const;
    void setSortFlag(const QList<qint32> &sortFlag);

    void setFilter(const QString &filter);
    QString filter() const;

    QJSValue dateConvertorMethod() const;
    void setDateConvertorMethod(const QJSValue &method);

    QJSValue messageTextMethod() const;
    void setMessageTextMethod(const QJSValue &method);

    void setUseCache(bool useCache);
    bool useCache() const;

    QVariantMap categories() const;
    void setCategories(const QVariantMap &categories);

    bool refreshing() const;

    QByteArray id(const QModelIndex &index) const;
    int count() const;

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QHash<int, QByteArray> roleNames() const;
    static QStringList requiredProperties();

Q_SIGNALS:
    void visibilityChanged();
    void sortFlagChanged();
    void filterChanged();
    void dateConvertorMethodChanged();
    void messageTextMethodChanged();
    void refreshingChanged();
    void categoriesChanged();
    void useCacheChanged();

public Q_SLOTS:
    int indexOf(InputPeerObject *peer);
    void clearHistory(InputPeerObject *peer, bool justClear = false, const QJSValue &callback = QJSValue());

protected:
    void refresh();
    void clean();
    void resort();

    void timerEvent(QTimerEvent *e);

    virtual QString statusText(const TelegramDialogListItem &item) const;
    virtual QString convertDate(const QDateTime &td) const;
    virtual QString messageText(class MessageObject *msg) const;

    virtual void connectTelegram();

private:
    void getDialogsFromServer(const class InputPeer &offset, int limit, QHash<QByteArray, TelegramDialogListItem> *items = 0);
    void getSecretChats();
    void getContactsFromServer();
    class InputPeer processOnResult(const class MessagesDialogs &result, QHash<QByteArray, TelegramDialogListItem> *items);
    void changed(const QHash<QByteArray, TelegramDialogListItem> &items);
    QList<QByteArray> getSortedList(const QHash<QByteArray, TelegramDialogListItem> &items);

    void connectChatSignals(const QByteArray &id, class ChatObject *chat);
    void connectUserSignals(const QByteArray &id, class UserObject *user);
    void connectMessageSignals(const QByteArray &id, class MessageObject *message);
    void connectDialogSignals(const QByteArray &id, class DialogObject *dialog);

    virtual void onUpdates(const UpdatesType &update);

    void insertUpdate(const Update &update);
    void setRefreshing(bool stt);

    void clearHistoryAnswer(qint64 msgId, const MessagesAffectedHistory &result);

private:
    TelegramDialogListModelPrivate *p;
};

#endif // TELEGRAMDIALOGLISTMODEL_H
