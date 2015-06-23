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

#include "abstractapi.h"

AbstractApi::AbstractApi(Session *session, Settings *settings, CryptoUtils *crypto, QObject *parent) :
    SessionManager(session, settings, crypto, parent) {
    // connect responses and updates signals in main session
    connectResponsesSignals(mMainSession);
    connectUpdatesSignals(mMainSession);
}

AbstractApi::~AbstractApi() {
}

void AbstractApi::connectUpdatesSignals(Session *session) {
    connect(session, SIGNAL(updatesTooLong()), this, SIGNAL(updatesTooLong()));
    connect(session, SIGNAL(updateShortMessage(qint32,qint32,const QString&,qint32,qint32,qint32,qint32,qint32,qint32,bool,bool)), this, SIGNAL(updateShortMessage(qint32,qint32,const QString&,qint32,qint32,qint32,qint32,qint32,qint32,bool,bool)));
    connect(session, SIGNAL(updateShortChatMessage(qint32,qint32,qint32,const QString&,qint32,qint32,qint32,qint32,qint32,qint32,bool,bool)), this, SIGNAL(updateShortChatMessage(qint32,qint32,qint32,const QString&,qint32,qint32,qint32,qint32,qint32,qint32,bool,bool)));
    connect(session, SIGNAL(updateShort(const Update&,qint32)), this, SIGNAL(updateShort(const Update&,qint32)));
    connect(session, SIGNAL(updatesCombined(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32,qint32)), this, SIGNAL(updatesCombined(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32,qint32)));
    connect(session, SIGNAL(updates(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32)), this, SIGNAL(updates(const QList<Update>&,const QList<User>&,const QList<Chat>&,qint32,qint32)));
}

void AbstractApi::connectResponsesSignals(Session *session) {
    connect(session, SIGNAL(resultReceived(Query*,InboundPkt&)), this, SLOT(onResultReceived(Query*,InboundPkt&)));
    connect(session, SIGNAL(errorReceived(Query*,qint32,QString)), this, SLOT(onErrorReceived(Query*,qint32,QString)));
}

/**
 * @brief AbstractApi::onErrorReceived manages any error received from the server and links to declared
 *  "onError" QueryMethod for current operation
 * @param q
 * @param errorCode
 * @param errorText
 */
void AbstractApi::onErrorReceived(Query *q, qint32 errorCode, QString errorText) {
    if (q->methods() && q->methods()->onError) {
        (((Api *)this)->*(q->methods()->onError))(q, errorCode, errorText);
    } else {
        onError(q, errorCode, errorText);
    }
    delete q;
}

/**
 * @brief AbstractApi::onResultReceived manages any positive response received from telegram servers
 *  and links to declared "onAnswer" QueryMethod for current operation
 * @param q
 * @param inboundPkt
 */
void AbstractApi::onResultReceived(Query *q, InboundPkt &inboundPkt) {
    if (q->methods() && q->methods()->onAnswer) {
        (((Api *)this)->*(q->methods()->onAnswer))(q, inboundPkt);
        Q_ASSERT(inboundPkt.inPtr() == inboundPkt.inEnd());
    }
    delete q;
}
