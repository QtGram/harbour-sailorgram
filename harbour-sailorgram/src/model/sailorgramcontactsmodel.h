#ifndef SAILORGRAMCONTACTSMODEL_H
#define SAILORGRAMCONTACTSMODEL_H

#include <QIdentityProxyModel>
#include <telegramqml.h>
#include <telegramcontactsmodel.h>
#include <objects/types.h>

class SailorgramContactsModel : public QIdentityProxyModel
{
    Q_OBJECT

    Q_PROPERTY(TelegramQml* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool initializing READ initializing NOTIFY initializingChanged)

    public:
        enum { ContactRole = Qt::UserRole + 10, UserRole = Qt::UserRole + 11, LetterRole = Qt::UserRole + 12 };

    public:
        explicit SailorgramContactsModel(QObject *parent = 0);
        TelegramQml* telegram() const;
        void setTelegram(TelegramQml* telegram);
        int count() const;
        bool initializing() const;

    public:
        virtual QVariant data(const QModelIndex &proxyIndex, int role) const;
        virtual QHash<int, QByteArray> roleNames() const;

    signals:
        void telegramChanged();
        void countChanged();
        void initializingChanged();

    private:
        TelegramContactsModel* _telegramcontactsmodel;
};

#endif // SAILORGRAMCONTACTSMODEL_H
