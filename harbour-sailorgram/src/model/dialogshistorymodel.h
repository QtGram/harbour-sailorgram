#ifndef DIALOGSHISTORYMODEL_H
#define DIALOGSHISTORYMODEL_H

#define MAX_RECENT_CHATS 4

#include <QSortFilterProxyModel>
#include <telegramdialogsmodel.h>

class DialogsHistoryModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(TelegramDialogsModel* dialogsModel READ dialogsModel WRITE setDialogsModel NOTIFY dialogsModelChanged)
    Q_PROPERTY(int historyLength READ historyLength WRITE setHistoryLength NOTIFY historyLengthChanged)

    public:
        TelegramDialogsModel* dialogsModel() const;
        void setDialogsModel(TelegramDialogsModel* dialogsmodel);
        int historyLength() const;
        void setHistoryLength(int hl);

    public:
        explicit DialogsHistoryModel(QObject *parent = 0);

    protected:
        virtual bool filterAcceptsRow(int source_row, const QModelIndex &) const;

    signals:
        void dialogsModelChanged();
        void historyLengthChanged();

    private:
        TelegramQml* _telegram;
        TelegramDialogsModel* _dialogsmodel;
        int _historylength;
};

#endif // DIALOGSHISTORYMODEL_H
