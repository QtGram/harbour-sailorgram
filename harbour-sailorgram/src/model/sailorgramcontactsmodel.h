#ifndef SAILORGRAMCONTACTSMODEL_H
#define SAILORGRAMCONTACTSMODEL_H

#include <QIdentityProxyModel>
#include <telegramdialoglistmodel.h>

class SailorgramContactsModel : public QIdentityProxyModel
{
    Q_OBJECT

    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

    public:
        enum { FirstLetterRole = Qt::UserRole + 100 };

    public:
        explicit SailorgramContactsModel(QObject *parent = 0);
        TelegramEngine *engine() const;
        void setEngine(TelegramEngine *engine);
        int count() const;

    public:
        virtual QVariant data(const QModelIndex &proxyIndex, int role) const;
        virtual QHash<int, QByteArray> roleNames() const;

    signals:
        void engineChanged();
        void countChanged();

    private:
        TelegramDialogListModel* _telegramdialoglistmodel;
};

#endif // SAILORGRAMCONTACTSMODEL_H
