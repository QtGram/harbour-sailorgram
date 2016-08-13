#ifndef SAILORGRAMDIALOGSMODEL_H
#define SAILORGRAMDIALOGSMODEL_H

#include <telegramdialoglistmodel.h>
#include "abstract/sailorgramidentityproxymodel.h"
#include "../item/sailorgramdialogitem.h"

class SailorgramDialogsModel : public SailorgramIdentityProxyModel
{
    Q_OBJECT

    private:
        enum DataRoles { RoleItem = Qt::UserRole };

    public:
        explicit SailorgramDialogsModel(QObject *parent = 0);
        virtual QVariant data(const QModelIndex &proxyindex, int role) const;
        virtual QHash<int, QByteArray> roleNames() const;

    protected:
        virtual void init(TelegramEngine *engine);
        virtual void inserted(QModelIndex sourceindex);
        virtual void removed(QModelIndex sourceindex);
        virtual void changed(QModelIndex sourceindex, const QVector<int> &roles);
        virtual void clear();
        virtual bool contains(QModelIndex sourceindex) const;

    private:
        void updateData(SailorgramDialogItem* sgdialog, const QModelIndex& sourceindex, const QVector<int>& roles);

    protected:
        TelegramDialogListModel* _dialoglistmodel;
        QHash<uint, SailorgramDialogItem*> _dialogs;
};

#endif // SAILORGRAMDIALOGSMODEL_H
