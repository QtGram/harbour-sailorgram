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
    connect(session, &Session::updates, this, &AbstractApi::updates);
}

void AbstractApi::connectResponsesSignals(Session *session) {
    connect(session, &Session::resultReceived, this, &AbstractApi::onResultReceived);
    connect(session, &Session::errorReceived, this, &AbstractApi::onErrorReceived);
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
        (((TelegramApi *)this)->*(q->methods()->onError))(q, errorCode, errorText);
    } else {
        bool accepted = false;
        onError(q, errorCode, errorText, q->extra(), accepted);
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
        (((TelegramApi *)this)->*(q->methods()->onAnswer))(q, inboundPkt);
        if(inboundPkt.inPtr() != inboundPkt.inEnd())
        {
            Q_EMIT fatalError();
            return;
        }
    }
    delete q;
}
