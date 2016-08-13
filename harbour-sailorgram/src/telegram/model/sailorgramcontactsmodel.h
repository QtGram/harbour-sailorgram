#ifndef SAILORGRAMCONTACTSMODEL_H
#define SAILORGRAMCONTACTSMODEL_H

#include "sailorgramdialogsmodel.h"

class SailorgramContactsModel : public SailorgramDialogsModel
{
    Q_OBJECT

    public:
        enum { RoleFirstLetter = Qt::UserRole + 10 };

    public:
        explicit SailorgramContactsModel(QObject *parent = 0);

    protected:
        virtual void init(TelegramEngine *engine);

    public:
        virtual QVariant data(const QModelIndex &proxyIndex, int role) const;
        virtual QHash<int, QByteArray> roleNames() const;
};

#endif // SAILORGRAMCONTACTSMODEL_H
