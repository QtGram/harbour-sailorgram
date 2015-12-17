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

#include "secretstate.h"
#include "core/settings.h"

SecretState::SecretState(Settings *settings) :
    mSettings(settings),
    mVersion(0),
    mG(0),
    mP(0) {
}

SecretState::~SecretState() {
    if (mG) BN_clear_free(mG);
    if (mP) BN_clear_free(mP);

    Q_FOREACH (SecretChat *chat, mChats.values()) {
        if (chat) {
            delete chat;
            chat = 0;
        }
    }
}

void SecretState::setG(qint32 g) {
    if (mG) BN_free(mG);
    mG = BN_new();
    Utils::ensurePtr(mG);
    Utils::ensure(BN_set_word(mG, g));
}

void SecretState::setP(const QByteArray &bigEndianP) {
    if (mP) BN_free(mP);
    mP = BN_bin2bn(reinterpret_cast<const uchar*>(bigEndianP.data()), 256, 0);
    Utils::ensurePtr(mP);
}


void SecretState::save() {
    mSettings->setVersion(mVersion);
    qint32 g = BN_get_word(mG);
    mSettings->setG(g);
    QByteArray mPBytes = Utils::bignumToBytes(mP);
    mSettings->setP(mPBytes);
    mSettings->setSecretChats(mChats.values());
    mSettings->writeSecretFile();
}

void SecretState::load() {
    mVersion = mSettings->version();
    setG(mSettings->g());
    setP(mSettings->p());
    mChats.clear();
    Q_FOREACH (SecretChat *secretChat, mSettings->secretChats()) {
        mChats.insert(secretChat->chatId(), secretChat);
    }
}
