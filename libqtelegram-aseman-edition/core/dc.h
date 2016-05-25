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

#ifndef NETWORKMGR_H
#define NETWORKMGR_H

#include <QObject>
#include "outboundpkt.h"
#include "inboundpkt.h"
#include "connection.h"
#include "settings.h"
#include "util/cryptoutils.h"

class DC : public Endpoint
{
public:
    enum DcState {
        error = -1,
        init,
        reqPQSent,
        reqDHSent,
        clientDHSent,
        authKeyCreated,
        userSignedIn
    };

    explicit DC(qint32 dcNum) :
        m_id(dcNum),
        m_state(init),
        m_authKeyId(0),
        m_expires(0),
        m_serverSalt(0),
        mTimeDifference(0),
        m_mediaOnly(false) {}

    inline qint32 id() { return m_id; }
    inline void setState(DcState dcState) { m_state = dcState; }
    inline DcState state() { return m_state; }
    inline qint64 serverSalt() { return m_serverSalt; }
    inline void setServerSalt(qint64 serverSalt) { this->m_serverSalt = serverSalt; }
    inline qint64 authKeyId() { return m_authKeyId; }
    inline void setAuthKeyId(qint64 authkeyId) { this->m_authKeyId = authkeyId; }
    inline char *authKey() { return m_authKey; }
    inline double timeDifference() { return mTimeDifference; }
    inline void setTimeDifference(qint32 timeDifference) { mTimeDifference = timeDifference; }
    inline qint32 expires() { return m_expires; }
    inline void setExpires(qint32 expires) { m_expires = expires; }
    inline bool mediaOnly() const { return m_mediaOnly; }
    inline void setMediaOnly(bool mediaOnly) { m_mediaOnly = mediaOnly; }

private:

    // dc metadata
    qint32 m_id;
    DcState m_state;
    // auth members
    qint64 m_authKeyId;
    char m_authKey[SHARED_KEY_LENGTH];
    qint32 m_expires; // date the authorization expires
    // to be used during session
    qint64 m_serverSalt;
    qint32 mTimeDifference; // difference between client and server time

    bool m_mediaOnly;
};

#endif // NETWORKMGR_H




