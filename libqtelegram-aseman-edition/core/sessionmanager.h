/*
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

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>
#include <QMap>
#include <QPointer>
#include <QLoggingCategory>
#include <QPointer>

#include "session.h"
#include "dc.h"

Q_DECLARE_LOGGING_CATEGORY(TG_CORE_SESSIONMANAGER)

class SessionManager : public QObject
{
    Q_OBJECT
public:
    explicit SessionManager(Session *session, Settings *settings, CryptoUtils *crypto, QObject *parent = 0);
    virtual ~SessionManager();

    void createMainSessionToDc(DC *dc);
    void changeMainSessionToDc(DC *dc);
    Session *mainSession();
    void setMainSession(Session *s);
    Session *fileSession(DC *dc);
    void resetFileSessions();

Q_SIGNALS:
    void mainSessionDcChanged(DC *dc);
    void mainSessionReady();
    void mainSessionClosed();

protected:
    QPointer<Settings> mSettings;
    QPointer<CryptoUtils> mCrypto;
    QPointer<Session> mMainSession;
    // sessionId -> Session object
    QMap<qint64, Session *> mFileSessions;
    // dcId -> sessionId
    QMap<qint32, qint64> mDcSessionIds;
    // dcId -> 'resources using' count (files being downloading using this session)
    QMap<qint32, qint32> mDcResourceCounts;

    Session *createSession(DC *dc);
    Session *createFileSession(DC *dc);

    virtual void connectResponsesSignals(Session *session) = 0;
    virtual void connectUpdatesSignals(Session *session) = 0;

private Q_SLOTS:
    void onSessionReleased(qint64 sessionId);
    void onSessionClosed(qint64 sessionId);
};

#endif // SESSIONMANAGER_H
