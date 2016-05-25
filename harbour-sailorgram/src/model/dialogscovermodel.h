#ifndef DIALOGSCOVERMODEL_H
#define DIALOGSCOVERMODEL_H

#include <QAbstractListModel>
#include <telegramdialoglistmodel.h>

class DialogsCoverModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(TelegramDialogListModel* dialogsModel READ dialogsModel WRITE setDialogsModel NOTIFY dialogsModelChanged)
    Q_PROPERTY(int maxDialogs READ maxDialogs WRITE setMaxDialogs NOTIFY maxDialogsChanged)

    public:
        enum DialogsCoverModelRoles { ItemRole = Qt::UserRole };

    public:
        explicit DialogsCoverModel(QObject *parent = 0);
        TelegramDialogListModel* dialogsModel() const;
        void setDialogsModel(TelegramDialogListModel* dialoglistmodel);
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
        TelegramDialogListModel* _telegramdialoglistmodel;
        int _maxdialogs;
};

#endif // DIALOGSCOVERMODEL_H
