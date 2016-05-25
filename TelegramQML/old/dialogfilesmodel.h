#ifndef DIALOGFILESMODEL_H
#define DIALOGFILESMODEL_H

#include "telegramqml_global.h"
#include "tgabstractlistmodel.h"

class DialogObject;
class TelegramQml;
class DialogFilesModelPrivate;
class TELEGRAMQMLSHARED_EXPORT DialogFilesModel : public TgAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(FileRoles)

    Q_PROPERTY(TelegramQml* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)
    Q_PROPERTY(DialogObject* dialog READ dialog WRITE setDialog NOTIFY dialogChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum FileRoles {
        NameRole = Qt::UserRole,
        PathRole,
        ThumbnailRole,
        SuffixRole
    };

    DialogFilesModel(QObject *parent = 0);
    ~DialogFilesModel();

    TelegramQml *telegram() const;
    void setTelegram(TelegramQml *tg );

    DialogObject *dialog() const;
    void setDialog( DialogObject *dlg );

    QString id( const QModelIndex &index ) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QHash<qint32,QByteArray> roleNames() const;

    int count() const;

public Q_SLOTS:
    void refresh();

Q_SIGNALS:
    void telegramChanged();
    void countChanged();
    void dialogChanged();

private:
    QString dirPath() const;

private:
    DialogFilesModelPrivate *p;
};

#endif // DIALOGFILESMODEL_H
