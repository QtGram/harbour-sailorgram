#ifndef SAILORGRAMMESSAGEMODEL_H
#define SAILORGRAMMESSAGEMODEL_H

#include <telegrammessagelistmodel.h>
#include "abstract/sailorgramidentityproxymodel.h"
#include "../item/sailorgrammessageitem.h"

class SailorgramMessageModel : public SailorgramIdentityProxyModel
{
    Q_OBJECT

    Q_PROPERTY(InputPeerObject* currentPeer READ currentPeer WRITE setCurrentPeer NOTIFY currentPeerChanged)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)

    public:
        enum DataRoles { RoleItem = Qt::UserRole };

    public:
        explicit SailorgramMessageModel(QObject *parent = 0);
        InputPeerObject* currentPeer() const;
        int limit() const;
        void setCurrentPeer(InputPeerObject* currentpeer);
        void setLimit(int limit);
        virtual QVariant data(const QModelIndex &proxyindex, int role) const;
        virtual QHash<int, QByteArray> roleNames() const;

    protected:
        virtual void init(TelegramEngine *engine);
        virtual void inserted(QModelIndex sourceindex);
        virtual void removed(QModelIndex sourceindex);
        virtual void changed(QModelIndex sourceindex, const QVector<int> &roles);
        virtual void clear();
        virtual bool contains(QModelIndex sourceindex) const;

    public slots:
        void markAsRead();

    private:
        void updateData(SailorgramMessageItem* sgmessage, const QModelIndex& sourceindex, const QVector<int>& roles);

    signals:
        void currentPeerChanged();
        void limitChanged();

    protected:
        TelegramMessageListModel* _messagelistmodel;
        QHash<uint, SailorgramMessageItem*> _messages;

    private:
        InputPeerObject* _currentpeer;
};

#endif // SAILORGRAMMESSAGEMODEL_H
