/*
 * Copyright 2014 Canonical Ltd.
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

#ifndef SECRETSTATE_H
#define SECRETSTATE_H

#include <QObject>
#include <QMap>
#include <openssl/bn.h>
#include "util/utils.h"
#include "types/inputuser.h"
#include "secretchat.h"

class SecretState : public QObject
{
    Q_OBJECT
public:
    explicit SecretState(Settings *settings, QObject *parent = 0);
    ~SecretState();

    qint32 version() { return mVersion; }
    void setVersion(qint32 version) { mVersion = version; }
    BIGNUM *g() { return mG; }
    BIGNUM *p() { return mP; }
    QMap<qint64, SecretChat *> &chats() { return mChats; }
    void setG(qint32 g);
    void setP(const QByteArray &bigEndianP);
    void setChats(const QMap<qint64, SecretChat *> &chats) { mChats = chats; }

    void load();
    void save();

private:
    Settings *mSettings;
    qint32 mVersion;
    BIGNUM *mG;
    BIGNUM *mP;
    QMap<qint64, SecretChat *> mChats;
};

#endif // SECRETSTATE_H
