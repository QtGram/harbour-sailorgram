#ifndef DIALOGSCOVERMODEL_H
#define DIALOGSCOVERMODEL_H

#include <QIdentityProxyModel>
#include <models/dialogsmodel.h>

class DialogsCoverModel : public QIdentityProxyModel
{
    Q_OBJECT

    Q_PROPERTY(DialogsModel* dialogsModel READ dialogsModel WRITE setDialogsModel NOTIFY dialogsModelChanged)
    Q_PROPERTY(int maxDialogs READ maxDialogs WRITE setMaxDialogs NOTIFY maxDialogsChanged)

    public:
        explicit DialogsCoverModel(QObject *parent = 0);
        DialogsModel* dialogsModel() const;
        int maxDialogs() const;
        void setDialogsModel(DialogsModel* dialogsmodel);
        void setMaxDialogs(int maxdialogs);

    public:
        virtual int rowCount(const QModelIndex&) const;

    signals:
        void dialogsModelChanged();
        void maxDialogsChanged();

    private:
        DialogsModel* _dialogsmodel;
        int _maxdialogs;
};

#endif // DIALOGSCOVERMODEL_H
