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
    void updates(const UpdatesType &update);

protected:
    // default error method. Overwritten in inherited classes
    virtual void onError(Query *q, qint32 errorCode, const QString &errorText, const QVariant &attachedData, bool &accepted) = 0;

private:
    void connectUpdatesSignals(Session *session);
    void connectResponsesSignals(Session *session);

private Q_SLOTS:
    void onResultReceived(Query *q, InboundPkt &inboundPkt);
    void onErrorReceived(Query *q, qint32 errorCode, QString errorText);
};

#endif // ABSTRACTAPI_H
