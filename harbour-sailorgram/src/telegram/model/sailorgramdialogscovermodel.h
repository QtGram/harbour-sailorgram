#ifndef SAILORGRAMDIALOGSCOVERMODEL_H
#define SAILORGRAMDIALOGSCOVERMODEL_H

#include <QAbstractListModel>
#include "sailorgramdialogsmodel.h"

class SailorgramDialogsCoverModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(SailorgramDialogsModel* dialogsModel READ dialogsModel WRITE setDialogsModel NOTIFY dialogsModelChanged)
    Q_PROPERTY(int maxDialogs READ maxDialogs WRITE setMaxDialogs NOTIFY maxDialogsChanged)

    public:
        explicit SailorgramDialogsCoverModel(QObject *parent = 0);
        SailorgramDialogsModel* dialogsModel() const;
        void setDialogsModel(SailorgramDialogsModel *dialoglistmodel);
        int maxDialogs() const;
        void setMaxDialogs(int i);

    public:
        virtual QHash<int, QByteArray> roleNames() const;
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual int rowCount(const QModelIndex&) const;

    signals:
        void dialogsModelChanged();
        void maxDialogsChanged();

    private:
        SailorgramDialogsModel* _dialoglistmodel;
        int _maxdialogs;
};

#endif // SAILORGRAMDIALOGSCOVERMODEL_H
