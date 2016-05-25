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

#ifndef QUERYMETHODS_H
#define QUERYMETHODS_H

#include "inboundpkt.h"

class TelegramApi;
class Query;

class QueryMethods{

public:

    QueryMethods() :
        onAnswer(0),
        onError(0),
        onTimeout(0) {
    }

    void (TelegramApi::*onAnswer)(Query *q, InboundPkt &inboundPkt);
    void (TelegramApi::*onError)(Query *q, qint32 errorCode, const QString &errorText);
    void (TelegramApi::*onTimeout)(Query *q);
};

#endif // QUERYMETHODS_H
