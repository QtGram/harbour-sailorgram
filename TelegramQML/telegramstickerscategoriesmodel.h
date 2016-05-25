#ifndef TELEGRAMSTICKERSCATEGORIESMODEL_H
#define TELEGRAMSTICKERSCATEGORIESMODEL_H

#include "telegramqml_macros.h"
#include "telegramqml_global.h"
#include "telegramabstractenginelistmodel.h"

class TelegramStickersCategoriesItem;
class TelegramStickersCategoriesModelPrivate;
class TelegramStickersCategoriesModel : public TelegramAbstractEngineListModel
{
    Q_OBJECT
    Q_PROPERTY(bool refreshing READ refreshing NOTIFY refreshingChanged)

public:
    enum DataRoles {
        RoleStickerSetItem = Qt::UserRole,
        RoleInstalled,
        RoleDisabled,
        RoleOfficial,
        RoleTitle,
        RoleShortName,
        RoleCount,
        RoleHash,
        RoleInputItem,
        RoleDocumentItems
    };

    TelegramStickersCategoriesModel(QObject *parent = 0);
    ~TelegramStickersCategoriesModel();

    bool refreshing() const;

    QByteArray id(const QModelIndex &index) const;
    int count() const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;
    static QStringList requiredProperties();

Q_SIGNALS:
    void refreshingChanged();

public Q_SLOTS:

protected:
    void refresh();
    void clean();
    void getStickersFromServer();

    void setRefreshing(bool refreshing);

private:
    void changed(const QList<QByteArray> &order, const QHash<QByteArray, TelegramStickersCategoriesItem> &items);

private:
    TelegramStickersCategoriesModelPrivate *p;
};

#endif // TELEGRAMSTICKERSCATEGORIESMODEL_H
