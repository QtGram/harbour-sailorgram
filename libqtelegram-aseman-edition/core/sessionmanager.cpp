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

#include "sessionmanager.h"

Q_LOGGING_CATEGORY(TG_CORE_SESSIONMANAGER, "tg.core.sessionmanager")

SessionManager::SessionManager(Session *session, Settings *settings, CryptoUtils *crypto, QObject *parent) :
    QObject(parent),
    mSettings(settings),
    mCrypto(crypto),
    mMainSession(session)
{
    connect(mMainSession.data(), &Session::sessionReady, this, &SessionManager::mainSessionReady, Qt::UniqueConnection);
    connect(mMainSession.data(), &Session::sessionClosed, this, &SessionManager::mainSessionClosed, Qt::UniqueConnection);
}

SessionManager::~SessionManager() {
    if (mMainSession) {
        mMainSession->close();
    }
    Q_FOREACH (Session *session, mFileSessions) {
        if (session) {
            session->close();
        }
    }
}

Session *SessionManager::fileSession(DC *dc) {
    Session *session;
    qint32 &resourcesAtDc = mDcResourceCounts[dc->id()];

    if (!resourcesAtDc) {
        session = createFileSession(dc);
    } else {
        qint64 sessionId = mDcSessionIds.value(dc->id());
        session = mFileSessions.value(sessionId);
    }

    resourcesAtDc++;
    qCDebug(TG_CORE_SESSIONMANAGER) << "file session resources at DC" << dc->id() << resourcesAtDc;

    return session;
}

Session *SessionManager::createFileSession(DC *dc) {
    Session *session = createSession(dc);
    mFileSessions.insert(session->sessionId(), session);
    mDcSessionIds.insert(dc->id(), session->sessionId());
    qCDebug(TG_CORE_SESSIONMANAGER) << "created file session at DC" << dc->id();
    return session;
}

Session *SessionManager::createSession(DC *dc) {
    Session *session = new Session(dc, mSettings, mCrypto, this);
    connect(session, &Session::sessionReleased, this, &SessionManager::onSessionReleased);
    connect(session, &Session::sessionClosed, this, &SessionManager::onSessionClosed);
    connectResponsesSignals(session);
    return session;
}

void SessionManager::onSessionReleased(qint64 sessionId) {
    Session *session = mFileSessions.value(sessionId, 0);
    if (session) {
        qint32 dcId = session->dc()->id();
        qint32 &resourcesAtDc = mDcResourceCounts[dcId];
        resourcesAtDc--;

        if (!resourcesAtDc) {
            session->close();
            mDcSessionIds.remove(dcId);
            qCDebug(TG_CORE_SESSIONMANAGER) << "closed file session at DC" << dcId;
            qCDebug(TG_CORE_SESSIONMANAGER) << "remaining" << mFileSessions.count() << "file sessions, counting all DCs";
        } else {
            qCDebug(TG_CORE_SESSIONMANAGER) << "released file session at DC" << dcId << ", remaining" << resourcesAtDc << "resources";
        }
    } else if (mMainSession && mMainSession->sessionId() == sessionId) {
        mMainSession->close();
    }
}

void SessionManager::onSessionClosed(qint64 sessionId) {
    Session *session = mFileSessions.take(sessionId);
    if (session) {
        session->deleteLater();
    } else if (mMainSession && mMainSession->sessionId() == sessionId) {
        mMainSession->deleteLater();
    }
}

Session *SessionManager::mainSession() {
    return mMainSession;
}

void SessionManager::setMainSession(Session *session) {
    mMainSession = session;
}

void SessionManager::changeMainSessionToDc(DC *dc) {
    // remove current main session that is connected to a wrong dc
    if (mMainSession) {
        mMainSession->close();
    }
    // create session and connect to dc, adding the signal of dc changed
    createMainSessionToDc(dc);
    connect(mMainSession.data(), &Session::sessionReady, this, &SessionManager::mainSessionDcChanged, Qt::UniqueConnection);
}

void SessionManager::createMainSessionToDc(DC *dc) {
    // create session and connect to dc
    mMainSession = createSession(dc);
    connect(mMainSession.data(), &Session::sessionReady, this, &SessionManager::mainSessionReady, Qt::UniqueConnection);
    connect(mMainSession.data(), &Session::sessionClosed, this, &SessionManager::mainSessionClosed, Qt::UniqueConnection);
    connectUpdatesSignals(mMainSession);
    mMainSession->connectToServer();
}
