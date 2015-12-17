#ifndef USERNAMEFILTERMODEL_H
#define USERNAMEFILTERMODEL_H

#include "telegramqml_global.h"
#include "tgabstractlistmodel.h"

class DialogObject;
class TelegramQml;
class UserNameFilterModelPrivate;
class TELEGRAMQMLSHARED_EXPORT UserNameFilterModel : public TgAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(FilterRoles)

    Q_PROPERTY(TelegramQml* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)
    Q_PROPERTY(DialogObject* dialog READ dialog WRITE setDialog NOTIFY dialogChanged)
    Q_PROPERTY(QString keyword READ keyword WRITE setKeyword NOTIFY keywordChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum FilterRoles {
        UserIdRole = Qt::UserRole
    };

    UserNameFilterModel(QObject *parent = 0);
    ~UserNameFilterModel();

    TelegramQml *telegram() const;
    void setTelegram(TelegramQml *tg );

    DialogObject *dialog() const;
    void setDialog(DialogObject *dialog);

    qint64 id( const QModelIndex &index ) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QHash<qint32,QByteArray> roleNames() const;

    int count() const;

    void setKeyword(const QString &keyword);
    QString keyword() const;

public Q_SLOTS:
    void refresh();
    qint64 get(int idx);

Q_SIGNALS:
    void telegramChanged();
    void countChanged();
    void keywordChanged();
    void dialogChanged();

private Q_SLOTS:
    void listChanged();

private:
    UserNameFilterModelPrivate *p;
};

#endif // USERNAMEFILTERMODEL_H
