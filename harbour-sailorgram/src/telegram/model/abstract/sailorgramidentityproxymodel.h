#ifndef SAILORGRAMIDENTITYPROXYMODEL_H
#define SAILORGRAMIDENTITYPROXYMODEL_H

#define SOURCE_ROLE_DATA(T, idx, role) sourceModel()->data(idx, role).value<T>()
#define ROLE_LIST(roles) (QVector<int>() << roles)

#include <QIdentityProxyModel>
#include <QVector>
#include <telegramengine.h>

class SailorgramIdentityProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)

    public:
        explicit SailorgramIdentityProxyModel(QObject *parent = 0);
        TelegramEngine* engine() const;
        void setEngine(TelegramEngine* engine);

    protected:
        virtual void init(TelegramEngine* engine) = 0;
        virtual void changed(QModelIndex sourceindex, const QVector<int>& roles) = 0;
        virtual void inserted(QModelIndex sourceindex);
        virtual void removed(QModelIndex sourceindex);
        void bindTo(QAbstractListModel* model);

    private slots:
        void onRowsInserted(const QModelIndex&, int start, int end);
        void onRowsRemoved(const QModelIndex&, int start, int end);
        void onDataChanged(const QModelIndex& topleft, const QModelIndex& bottomright, const QVector<int>& roles);

    signals:
        void engineChanged();
        void countChanged();

    protected:
        TelegramEngine* _engine;
};

#endif // SAILORGRAMIDENTITYPROXYMODEL_H
