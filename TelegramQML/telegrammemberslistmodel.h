#ifndef TELEGRAMMEMBERSLISTMODEL_H
#define TELEGRAMMEMBERSLISTMODEL_H

#include "telegramabstractenginelistmodel.h"

class InputPeerObject;
class TelegramMembersListModelPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramMembersListModel : public TelegramAbstractEngineListModel
{
    Q_OBJECT
    Q_ENUMS(Types)
    Q_ENUMS(DataRoles)
    Q_PROPERTY(InputPeerObject* currentPeer READ currentPeer WRITE setCurrentPeer NOTIFY currentPeerChanged)
    Q_PROPERTY(qint32 filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(bool refreshing READ refreshing NOTIFY refreshingChanged)
    Q_PROPERTY(QJSValue dateConvertorMethod READ dateConvertorMethod WRITE setDateConvertorMethod NOTIFY dateConvertorMethodChanged)

public:
    enum DataRoles {
        RoleDate = Qt::UserRole,
        RoleInviter,
        RoleUser,
        RoleKickedBy,
        RoleType,
        RolePeer,
        RoleStatus
    };

    enum Types {
        TypeNormal,
        TypeSelf,
        TypeAdmin,
        TypeModerator,
        TypeEditor,
        TypeKicked,
        TypeCreator,
        TypeUnknown
    };

    TelegramMembersListModel(QObject *parent = 0);
    ~TelegramMembersListModel();

    void setCurrentPeer(InputPeerObject *peer);
    InputPeerObject *currentPeer() const;

    void setFilter(qint32 filter);
    qint32 filter() const;

    void setRefreshing(bool refreshing);
    bool refreshing() const;

    QJSValue dateConvertorMethod() const;
    void setDateConvertorMethod(const QJSValue &method);

    class TelegramChatsMemebrsListModelItem id(const QModelIndex &index) const;
    int count() const;

    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;
    static QStringList requiredProperties();

Q_SIGNALS:
    void currentPeerChanged();
    void filterChanged();
    void refreshingChanged();
    void dateConvertorMethodChanged();

public Q_SLOTS:

private:
    void refresh();
    void clean();

    void changed(const QList<class TelegramChatsMemebrsListModelItem> &list);

protected:
    QString convertDate(const QDateTime &td) const;

private:
    TelegramMembersListModelPrivate *p;
};

#endif // TELEGRAMMEMBERSLISTMODEL_H
