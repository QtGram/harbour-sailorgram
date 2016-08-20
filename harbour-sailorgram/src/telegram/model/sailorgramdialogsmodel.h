#ifndef SAILORGRAMDIALOGSMODEL_H
#define SAILORGRAMDIALOGSMODEL_H

#include <telegramdialoglistmodel.h>
#include "abstract/sailorgramidentityproxymodel.h"
#include "../item/sailorgramdialogitem.h"

class SailorgramDialogsModel : public SailorgramIdentityProxyModel
{
    Q_OBJECT

    Q_PROPERTY(bool writableOnly READ writableOnly WRITE setWritableOnly NOTIFY writableOnlyChanged)

    private:
        enum DataRoles { RoleItem = Qt::UserRole };

    public:
        explicit SailorgramDialogsModel(QObject *parent = 0);
        bool writableOnly() const;
        void setWritableOnly(bool b);
        virtual QVariant data(const QModelIndex &proxyindex, int role) const;
        virtual QHash<int, QByteArray> roleNames() const;

    public slots:
        SailorgramDialogItem* dialog(const QByteArray &peerkey);
        void deleteDialog(SailorgramDialogItem* sgdialog);

    protected:
        virtual void init(TelegramEngine *engine);
        virtual void inserted(QModelIndex sourceindex);
        virtual void removed(QModelIndex sourceindex);
        virtual void changed(QModelIndex sourceindex, const QVector<int> &roles);
        virtual void clear();
        virtual bool contains(QModelIndex sourceindex) const;

    private:
        void updateData(SailorgramDialogItem* sgdialog, const QModelIndex& sourceindex, const QVector<int>& roles);
        void updateVisibility();

    signals:
        void writableOnlyChanged();

    protected:
        TelegramDialogListModel* _dialoglistmodel;
        QHash<QByteArray, SailorgramDialogItem*> _dialogs;

    private:
        bool _writableonly;
};

#endif // SAILORGRAMDIALOGSMODEL_H
