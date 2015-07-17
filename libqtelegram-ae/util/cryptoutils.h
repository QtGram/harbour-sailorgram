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

#ifndef CRYPTOUTILS_H
#define CRYPTOUTILS_H

#include <QObject>
#include "core/outboundpkt.h"

#include <bn.h>
#include <aes.h>

class CryptoUtils : public QObject
{
    Q_OBJECT
public:
    CryptoUtils(Settings *settings);
    ~CryptoUtils();
    CryptoUtils(const CryptoUtils &);
    CryptoUtils &operator=(const CryptoUtils &);

    qint32 encryptPacketBuffer(OutboundPkt &p, void *encryptBuffer);
    qint32 encryptPacketBufferAESUnAuth(const char serverNonce[16], const char hiddenClientNonce[32], OutboundPkt &p, void *encryptBuffer);
    qint32 padRsaEncrypt(char *from, qint32 from_len, char *to, qint32 size, BIGNUM *N, BIGNUM *E);

    void initAESAuth (char authKey[192], char msgKey[16], qint32 encrypt);
    void initAESUnAuth(const char serverNonce[16], const char hiddenClientNonce[32], qint32 encrypt);
    qint32 padAESDecrypt(const char *from, qint32 fromLen, char *to, qint32 size);
    qint32 padAESEncrypt(const char *from, qint32 fromLen, char *to, qint32 size);

    qint32 checkPrime(BIGNUM *p);
    qint32 checkDHParams(BIGNUM *p, qint32 g);
    qint32 checkCalculatedParams(const BIGNUM *gAOrB, const BIGNUM *g, const BIGNUM *p);

    qint32 BNModExp(BIGNUM *r, const BIGNUM *a, const BIGNUM *p, const BIGNUM *m);

    QByteArray encryptFilePart(const QByteArray &partBytes, uchar *key, uchar *iv);
    QByteArray decryptFilePart(const QByteArray &partBytes, uchar *key, uchar *iv);
    qint32 computeKeyFingerprint(const QByteArray &key, const QByteArray &iv);

private:
    Settings *mSettings;

    BN_CTX *BN_ctx;
    uchar aes_key_raw[32], aes_iv[32];
    AES_KEY aes_key;
};

#endif // CRYPTOUTILS_H
