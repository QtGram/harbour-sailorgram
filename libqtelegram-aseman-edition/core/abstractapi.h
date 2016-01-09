/*
 * Copyright 2013 Vitaly Valtman
 * Copyright 2014 Canonical Ltd.
 * Authors:
 *      Roberto Mier
 *      Tiago Herrmann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ABSTRACTAPI_H
#define ABSTRACTAPI_H

#include <QObject>
#include <QMap>
#include "telegram/types/update.h"
#include "session.h"
#include "sessionmanager.h"

class AbstractApi : public SessionManager
{
    Q_OBJECT
public:
    explicit AbstractApi(Session *session, Settings *settings, CryptoUtils *crypto, QObject *parent = 0);
    virtual ~AbstractApi();

Q_SIGNALS:
    void fatalError();
    void updatesTooLong();
    void updateShortMessage(qint32 id, qint32 userId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, qint32 fwd_from_id, qint32 fwd_date, qint32 reply_to_msg_id, bool unread, bool out);
    void updateShortChatMessage(qint32 id, qint32 fromId, qint32 chatId, const QString &message, qint32 pts, qint32 pts_count, qint32 date, qint32 fwd_from_id, qint32 fwd_date, qint32 reply_to_msg_id, bool unread, bool out);
    void updateShort(const Update &update, qint32 date);
    void updatesCombined(const QList<Update> &updates, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seqStart, qint32 seq);
    void updates(const QList<Update> &udts, const QList<User> &users, const QList<Chat> &chats, qint32 date, qint32 seq);

protected:
    // default error method. Overwritten in inherited classes
    virtual void onError(Query *q, qint32 errorCode, const QString &errorText) = 0;

private:
    void connectUpdatesSignals(Session *session);
    void connectResponsesSignals(Session *session);

private Q_SLOTS:
    void onResultReceived(Query *q, InboundPkt &inboundPkt);
    void onErrorReceived(Query *q, qint32 errorCode, QString errorText);
};

#endif // ABSTRACTAPI_H
