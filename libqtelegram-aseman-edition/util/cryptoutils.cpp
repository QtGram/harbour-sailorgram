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

#include "cryptoutils.h"

#include "core/settings.h"
#include "utils.h"
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/md5.h>

CryptoUtils::CryptoUtils(Settings *settings) :
    mSettings(settings)
{
    BN_ctx = BN_CTX_new ();
    Utils::ensurePtr(BN_ctx);
}

CryptoUtils::~CryptoUtils() {
    if (BN_ctx) free(BN_ctx); //do this with a tfree method from Utils
}

qint32 CryptoUtils::encryptPacketBuffer(OutboundPkt &p, void *encryptBuffer) {
    RSA *pubKey = mSettings->pubKey();
    return padRsaEncrypt((char *) p.buffer(), p.length() * 4, (char *) encryptBuffer, ENCRYPT_BUFFER_INTS * 4, pubKey->n, pubKey->e);
}

qint32 CryptoUtils::encryptPacketBufferAESUnAuth(const char serverNonce[16], const char hiddenClientNonce[32], OutboundPkt &p, void *encryptBuffer) {
  initAESUnAuth (serverNonce, hiddenClientNonce, AES_ENCRYPT);
  return padAESEncrypt ((char *) p.buffer(), p.length() * 4, (char *) encryptBuffer, ENCRYPT_BUFFER_INTS * 4);
}

qint32 CryptoUtils::padRsaEncrypt (char *from, qint32 from_len, char *to, qint32 size, BIGNUM *N, BIGNUM *E) {
    qint32 pad = (255000 - from_len - 32) % 255 + 32;
    qint32 chunks = (from_len + pad) / 255;
    qint32 bits = BN_num_bits (N);
    qint32 isSupported = RAND_pseudo_bytes ((uchar *) from + from_len, pad); // returns -1 if not supported
    Q_UNUSED(bits);
    Q_ASSERT(bits >= 2041 && bits <= 2048);
    Q_ASSERT(from_len > 0 && from_len <= 2550);
    Q_UNUSED(size);
    Q_ASSERT(size >= chunks * 256);
    Q_UNUSED(isSupported);
    Q_ASSERT(isSupported >= 0);
    qint32 i;
    BIGNUM x, y;
    BN_init (&x);
    BN_init (&y);
    for (i = 0; i < chunks; i++) {
        BN_bin2bn ((uchar *) from, 255, &x);
        qint32 success = BN_mod_exp (&y, &x, E, N, BN_ctx);
        Q_UNUSED(success);
        Q_ASSERT(success == 1);
        unsigned l = 256 - BN_num_bytes (&y);
        Q_ASSERT(l <= 256);
        memset (to, 0, l);
        BN_bn2bin (&y, (uchar *) to + l);
        to += 256;
    }
    BN_free (&x);
    BN_free (&y);
    return chunks * 256;
}

void CryptoUtils::initAESUnAuth (const char serverNonce[16], const char hiddenClientNonce[32], qint32 encrypt) {
    static uchar buffer[64], hash[20];
    memcpy (buffer, hiddenClientNonce, 32);
    memcpy (buffer + 32, serverNonce, 16);
    SHA1 (buffer, 48, aes_key_raw);
    memcpy (buffer + 32, hiddenClientNonce, 32);
    SHA1 (buffer, 64, aes_iv + 8);
    memcpy (buffer, serverNonce, 16);
    memcpy (buffer + 16, hiddenClientNonce, 32);
    SHA1 (buffer, 48, hash);
    memcpy (aes_key_raw + 20, hash, 12);
    memcpy (aes_iv, hash + 12, 8);
    memcpy (aes_iv + 28, hiddenClientNonce, 4);
    if (encrypt == AES_ENCRYPT) {
        AES_set_encrypt_key (aes_key_raw, 32*8, &aes_key);
    } else {
        AES_set_decrypt_key (aes_key_raw, 32*8, &aes_key);
    }
    Utils::secureZeroMemory (aes_key_raw, 0, sizeof (aes_key_raw));
}

void CryptoUtils::initAESAuth (char authKey[192], char msgKey[16], qint32 encrypt) {
    static uchar buffer[48], hash[20];
    /*
    Steps:
    1) sha1_a = SHA1 (msg_key + substr (auth_key, 0, 32));
    2) sha1_b = SHA1 (substr (auth_key, 32, 16) + msg_key + substr (auth_key, 48, 16));
    3) sha1_с = SHA1 (substr (auth_key, 64, 32) + msg_key);
    4) sha1_d = SHA1 (msg_key + substr (auth_key, 96, 32));
    5) aes_key = substr (sha1_a, 0, 8) + substr (sha1_b, 8, 12) + substr (sha1_c, 4, 12);
    6) aes_iv = substr (sha1_a, 8, 12) + substr (sha1_b, 0, 8) + substr (sha1_c, 16, 4) + substr (sha1_d, 0, 8);
    */
    memcpy (buffer, msgKey, 16);
    memcpy (buffer + 16, authKey, 32);
    SHA1 (buffer, 48, hash);
    memcpy (aes_key_raw, hash, 8);
    memcpy (aes_iv, hash + 8, 12);

    memcpy (buffer, authKey + 32, 16);
    memcpy (buffer + 16, msgKey, 16);
    memcpy (buffer + 32, authKey + 48, 16);
    SHA1 (buffer, 48, hash);
    memcpy (aes_key_raw + 8, hash + 8, 12);
    memcpy (aes_iv + 12, hash, 8);

    memcpy (buffer, authKey + 64, 32);
    memcpy (buffer + 32, msgKey, 16);
    SHA1 (buffer, 48, hash);
    memcpy (aes_key_raw + 20, hash + 4, 12);
    memcpy (aes_iv + 20, hash + 16, 4);

    memcpy (buffer, msgKey, 16);
    memcpy (buffer + 16, authKey + 96, 32);
    SHA1 (buffer, 48, hash);
    memcpy (aes_iv + 24, hash, 8);

    if (encrypt == AES_ENCRYPT) {
        AES_set_encrypt_key (aes_key_raw, 32*8, &aes_key);
    } else {
        AES_set_decrypt_key (aes_key_raw, 32*8, &aes_key);
    }
    Utils::secureZeroMemory (aes_key_raw, 0, sizeof (aes_key_raw));
}

qint32 CryptoUtils::padAESEncrypt (const char *from, qint32 fromLen, char *to, qint32 size) {
    qint32 paddedSize = (fromLen + 15) & -16;
    Q_UNUSED(size);
    Q_ASSERT(fromLen > 0 && paddedSize <= size);
    if (fromLen < paddedSize) {
        qint32 isRandSupported = RAND_pseudo_bytes ((uchar *) from + fromLen, paddedSize - fromLen);
        Q_UNUSED(isRandSupported);
        Q_ASSERT(isRandSupported >= 0);
    }
    AES_ige_encrypt ((uchar *) from, (uchar *) to, paddedSize, &aes_key, aes_iv, AES_ENCRYPT);
    return paddedSize;
}

qint32 CryptoUtils::padAESDecrypt (const char *from, qint32 fromLen, char *to, qint32 size) {
    if (fromLen <= 0 || fromLen > size || (fromLen & 15)) {
        return -1;
    }
    AES_ige_encrypt ((const uchar *) from, (uchar *) to, fromLen, &aes_key, aes_iv, AES_DECRYPT);
    return fromLen;
}

qint32 CryptoUtils::checkPrime (BIGNUM *p) {
    qint32 r = BN_is_prime (p, BN_prime_checks, 0, BN_ctx, 0);
    Utils::ensure (r >= 0);
    return r;
}

qint32 CryptoUtils::checkDHParams (BIGNUM *p, qint32 g) {
    if (g < 2 || g > 7) { return -1; }
    BIGNUM t;
    BN_init (&t);

    BIGNUM dh_g;
    BN_init (&dh_g);
    Utils::ensure (BN_set_word (&dh_g, 4 * g));

    Utils::ensure (BN_mod (&t, p, &dh_g, BN_ctx));
    qint32 x = BN_get_word (&t);
    Q_ASSERT(x >= 0 && x < 4 * g);

    BN_free (&dh_g);

    switch (g) {
    case 2:
        if (x != 7) { return -1; }
        break;
    case 3:
        if (x % 3 != 2 ) { return -1; }
        break;
    case 4:
        break;
    case 5:
        if (x % 5 != 1 && x % 5 != 4) { return -1; }
        break;
    case 6:
        if (x != 19 && x != 23) { return -1; }
        break;
    case 7:
        if (x % 7 != 3 && x % 7 != 5 && x % 7 != 6) { return -1; }
        break;
    }

    if (!checkPrime (p)) { return -1; }

    BIGNUM b;
    BN_init (&b);
    Utils::ensure (BN_set_word (&b, 2));
    Utils::ensure (BN_div (&t, 0, p, &b, BN_ctx));
    if (!checkPrime (&t)) { return -1; }
    BN_free (&b);
    BN_free (&t);
    return 0;
}

/**
  https://core.telegram.org/api/end-to-end says:
  "Both clients in a secret chat creation are to check that g, g_a and g_b are greater than one and smaller than p-1.
  Recommented checking that g_a and g_b are between 2^{2048-64} and p - 2^{2048-64} as well."
*/
qint32 CryptoUtils::checkCalculatedParams(const BIGNUM *gAOrB, const BIGNUM *g, const BIGNUM *p) {
    ASSERT(gAOrB);
    ASSERT(g);
    ASSERT(p);

    // 1) gAOrB and g greater than one and smaller than p-1
    BIGNUM one;
    BN_init(&one);
    Utils::ensure(BN_one(&one));

    BIGNUM *pMinusOne = BN_dup(p);
    Utils::ensure(BN_sub_word(pMinusOne, 1));

    // check params greater than one
    if (BN_cmp(gAOrB, &one) <= 0) return -1;
    if (BN_cmp(g, &one) <= 0) return -1;

    // check params <= p-1
    if (BN_cmp(gAOrB, pMinusOne) >= 0) return -1;
    if (BN_cmp(g, pMinusOne) >= 0) return -1;

    // 2) gAOrB between 2^{2048-64} and p - 2^{2048-64}
    quint64 expWord = 2048 - 64;
    BIGNUM exp;
    BN_init(&exp);
    Utils::ensure(BN_set_word(&exp, expWord));

    BIGNUM base;
    BN_init(&base);
    Utils::ensure(BN_set_word(&base, 2));

    // lowLimit = base ^ exp
    BIGNUM lowLimit;
    BN_init(&lowLimit);
    Utils::ensure(BN_exp(&lowLimit, &base, &exp, BN_ctx));

    // highLimit = p - lowLimit
    BIGNUM highLimit;
    BN_init(&highLimit);
    BN_sub(&highLimit, p, &lowLimit);

    if (BN_cmp(gAOrB, &lowLimit) < 0) return -1;
    if (BN_cmp(gAOrB, &highLimit) > 0) return -1;

    BN_free(&one);
    BN_free(pMinusOne);
    BN_free(&exp);
    BN_free(&lowLimit);
    BN_free(&highLimit);

    return 0;
}

qint32 CryptoUtils::BNModExp(BIGNUM *r, const BIGNUM *a, const BIGNUM *p, const BIGNUM *m) {
    return BN_mod_exp (r, a, p, m, BN_ctx);
}

QByteArray CryptoUtils::encryptFilePart(const QByteArray &partBytes, uchar *key, uchar *iv) {
    qint32 length = partBytes.length();
    uchar *buffer = (uchar *)partBytes.constData();
    // Use an output buffer for not to modify original one.
    // Max size of the output buffer, included padding, will be length + 15
    QScopedArrayPointer<uchar> out(new uchar[length + 15]);
    memcpy(out.data(), buffer, length);

    // in case this part length % 0xF != 0, it means is the last part and we must pad the size to match AES block size (16)
    qint32 paddedSize = length;
    if (length & 15) {
        paddedSize = (length + 15) & -16;
        if (length < paddedSize) {
            RAND_pseudo_bytes(out.data() + length, paddedSize - length);
        }
    }

    AES_KEY aesKey;
    AES_set_encrypt_key(key, 256, &aesKey);
    AES_ige_encrypt(out.data(), out.data(), paddedSize, &aesKey, iv, AES_ENCRYPT);
    Utils::secureZeroMemory(&aesKey, 0, sizeof(aesKey));

    return QByteArray((char *)out.data(), paddedSize);
}

QByteArray CryptoUtils::decryptFilePart(const QByteArray &partBytes, uchar *key, uchar *iv) {
    qint32 length = partBytes.length();
    uchar *buffer = (uchar *)partBytes.constData();

    ASSERT(!(length & 15));
    AES_KEY aesKey;
    AES_set_decrypt_key(key, 256, &aesKey);
    AES_ige_encrypt(buffer, buffer, length, &aesKey, iv, AES_DECRYPT);
    Utils::secureZeroMemory(&aesKey, 0, sizeof(aesKey));

    return QByteArray::fromRawData((char *)buffer, length);
}

qint32 CryptoUtils::computeKeyFingerprint(const QByteArray &keyBytes, const QByteArray &ivBytes) {
    uchar *key = (uchar *)keyBytes.data();
    uchar *iv = (uchar *)ivBytes.data();
    uchar md5[16];
    uchar str[64];
    memcpy(str, key, 32);
    memcpy(str + 32, iv, 32);
    MD5(str, 64, md5);
    qint32 keyFingerprint = (*(int *)md5) ^ (*(int *)(md5 + 4));
    return keyFingerprint;
}
