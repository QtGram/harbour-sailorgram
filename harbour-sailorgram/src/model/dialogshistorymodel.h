#ifndef DIALOGSHISTORYMODEL_H
#define DIALOGSHISTORYMODEL_H

#define MAX_RECENT_CHATS 4

#include <QAbstractListModel>
#include <QList>
#include <QSet>
#include <telegramqml.h>
#include <objects/types.h>

class DialogsHistoryModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(TelegramQml* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)

    public:
        enum DialogsHistoryModelRole { DialogRole = Qt::UserRole };
        TelegramQml* telegram() const;
        void setTelegram(TelegramQml* telegram);

    public:
        explicit DialogsHistoryModel(QObject *parent = 0);
        virtual QHash<int, QByteArray> roleNames() const;
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual int rowCount(const QModelIndex&) const;

    public slots:
        void markAsRead(DialogObject* dialog);

    private:
        void removeOlder();
        void removeDialog(DialogObject *dialog);

    private slots:
        void onIncomingMessage(MessageObject* message);

    signals:
        void telegramChanged();

    private:
        QList<DialogObject*> _recentdialogs;
        QSet<DialogObject*> _recentdialogsset;
        TelegramQml* _telegram;
};

#endif // DIALOGSHISTORYMODEL_H
