#ifndef DIALOGSCOVERMODEL_H
#define DIALOGSCOVERMODEL_H

#include <QAbstractListModel>
#include <telegramdialogsmodel.h>
#include <objects/types.h>

class DialogsCoverModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(TelegramDialogsModel* dialogsModel READ dialogsModel WRITE setDialogsModel NOTIFY dialogsModelChanged)
    Q_PROPERTY(int maxDialogs READ maxDialogs WRITE setMaxDialogs NOTIFY maxDialogsChanged)

    public:
        enum DialogsCoverModelRoles { ItemRole = Qt::UserRole };

    public:
        explicit DialogsCoverModel(QObject *parent = 0);
        TelegramDialogsModel* dialogsModel() const;
        void setDialogsModel(TelegramDialogsModel* dialogsmodel);
        int maxDialogs() const;
        void setMaxDialogs(int i);

    public:
        virtual QHash<int, QByteArray> roleNames() const;
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual int rowCount(const QModelIndex&) const;

    private slots:
        void onCountChanged();

    signals:
        void dialogsModelChanged();
        void maxDialogsChanged();

    private:
        TelegramDialogsModel* _dialogsmodel;
        int _maxdialogs;
};

#endif // DIALOGSCOVERMODEL_H
