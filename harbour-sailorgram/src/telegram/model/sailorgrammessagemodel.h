#ifndef SAILORGRAMMESSAGEMODEL_H
#define SAILORGRAMMESSAGEMODEL_H

#include <QTimer>
#include <telegrammessagelistmodel.h>
#include <telegramstatus.h>
#include "abstract/sailorgramidentityproxymodel.h"
#include "../item/sailorgrammessageitem.h"

class SailorgramMessageModel : public SailorgramIdentityProxyModel
{
    Q_OBJECT

    Q_PROPERTY(InputPeerObject* currentPeer READ currentPeer WRITE setCurrentPeer NOTIFY currentPeerChanged)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(int typingStatus READ typingStatus WRITE setTypingStatus NOTIFY typingStatusChanged)

    public:
        enum DataRoles { RoleItem = Qt::UserRole };

    public:
        explicit SailorgramMessageModel(QObject *parent = 0);
        InputPeerObject* currentPeer() const;
        int limit() const;
        int typingStatus() const;
        void setCurrentPeer(InputPeerObject* currentpeer);
        void setLimit(int limit);
        void setTypingStatus(int typingstatus);
        virtual QVariant data(const QModelIndex &proxyindex, int role) const;
        virtual QHash<int, QByteArray> roleNames() const;

    protected:
        virtual void init(TelegramEngine *engine);
        virtual void inserted(QModelIndex sourceindex);
        virtual void removed(QModelIndex sourceindex);
        virtual void changed(QModelIndex sourceindex, const QVector<int> &roles);
        virtual void clear();
        virtual bool contains(QModelIndex sourceindex) const;

    private slots:
        void onTypingTimerTimeout();

    public slots:
        void markAsRead();
        void sendMessage(const QString &message, SailorgramMessageItem *replyto = 0);
        void sendFile(int type, const QString& file, SailorgramMessageItem* replyto = 0);
        void deleteMessages(const QList<SailorgramMessageItem*> messages);
        void forwardMessages();

    private:
        void updateData(SailorgramMessageItem* sgmessage, const QModelIndex& sourceindex, const QVector<int>& roles);

    signals:
        void currentPeerChanged();
        void limitChanged();
        void typingStatusChanged();

    protected:
        TelegramMessageListModel* _messagelistmodel;
        QTimer* _typingtimer;
        TelegramStatus* _status;
        QHash<uint, SailorgramMessageItem*> _messages;

    private:
        InputPeerObject* _currentpeer;
};

#endif // SAILORGRAMMESSAGEMODEL_H
