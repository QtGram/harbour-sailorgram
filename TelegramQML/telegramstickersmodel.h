#ifndef TELEGRAMSTICKERSMODEL_H
#define TELEGRAMSTICKERSMODEL_H

#include <QStringList>

#include "telegramqml_macros.h"
#include "telegramqml_global.h"
#include "telegramabstractenginelistmodel.h"

class TelegramStickersItem;
class DocumentObject;
class InputStickerSetObject;
class TelegramEngine;
class TelegramStickersModelPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramStickersModel : public TelegramAbstractEngineListModel
{
    Q_OBJECT
    Q_ENUMS(DialogsRoles)
    Q_PROPERTY(bool refreshing READ refreshing NOTIFY refreshingChanged)
    Q_PROPERTY(QString shortName READ shortName WRITE setShortName NOTIFY shortNameChanged)
    Q_PROPERTY(InputStickerSetObject* stickerSet READ stickerSet WRITE setStickerSet NOTIFY stickerSetChanged)
    Q_PROPERTY(QVariantList documents READ documents WRITE setDocuments NOTIFY documentsChanged)

public:
    enum DialogsRoles {
        RoleDocument = Qt::UserRole,
        RoleAlt
    };

    TelegramStickersModel(QObject *parent = 0);
    ~TelegramStickersModel();

    QString shortName() const;
    void setShortName(const QString &shortName);

    void setStickerSet(InputStickerSetObject *stickerSet);
    InputStickerSetObject *stickerSet() const;

    void setDocuments(const QVariantList &list);
    QVariantList documents() const;

    QByteArray id( const QModelIndex &index ) const;
    int count() const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QHash<qint32,QByteArray> roleNames() const;

    bool refreshing() const;
    static QStringList requiredProperties();

public Q_SLOTS:
    void showRecents();
    void addToRecents(DocumentObject *doc);

Q_SIGNALS:
    void engineChanged();
    void refreshingChanged();
    void stickerSetChanged();
    void shortNameChanged();
    void documentsChanged();

protected:
    void refresh();
    void clean();
    void getFromServer();
    void changed(const QList<QByteArray> &list, const QHash<QByteArray, TelegramStickersItem> &items);

private:
    TelegramStickersModelPrivate *p;
};

#endif // TELEGRAMSTICKERSMODEL_H
