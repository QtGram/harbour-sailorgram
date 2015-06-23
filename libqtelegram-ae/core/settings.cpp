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

#include "settings.h"
#include "util/utils.h"
#include "util/constants.h"
#include "dc.h"

#include <QDir>
#include <QSettings>
#include <QDebug>
#include <fcntl.h>
#if !defined(Q_OS_WIN)
#include <unistd.h>
#endif

#include <openssl/bn.h>

Q_LOGGING_CATEGORY(TG_CORE_SETTINGS, "tg.core.settings")

Settings::Settings() :
    m_defaultHostPort(0),
    m_defaultHostDcId(0),
    m_appId(0),
    m_workingDcConfigAvailabe(false),
    m_pubKey(0),
    m_phoneNumber(""),
    m_baseConfigDirectory("") {
}

Settings::~Settings() {
    if (m_pubKey) {
        delete m_pubKey;
    }
}

void Settings::setDefaultHostAddress(const QString &host)
{
    m_defaultHostAddress = host;
}

void Settings::setDefaultHostPort(qint16 port)
{
    m_defaultHostPort = port;
}

void Settings::setDefaultHostDcId(qint16 dcId)
{
    m_defaultHostDcId = dcId;
}

void Settings::setAppId(qint32 appId)
{
    m_appId = appId;
}

void Settings::setAppHash(const QString &appHash)
{
    m_appHash = appHash;
}

QString Settings::defaultHostAddress()
{
    return m_defaultHostAddress;
}

qint16 Settings::defaultHostPort()
{
    return m_defaultHostPort;
}

qint16 Settings::defaultHostDcId()
{
    return m_defaultHostDcId;
}

qint32 Settings::appId()
{
    return m_appId;
}

QString Settings::appHash()
{
    return m_appHash;
}

bool Settings::loadSettings(const QString &phoneNumber, const QString &baseConfigDirectory, const QString &publicKeyFile) {

    m_phoneNumber = Utils::parsePhoneNumberDigits(phoneNumber);

    if (m_baseConfigDirectory.isEmpty()) {
        m_baseConfigDirectory = baseConfigDirectory;
    }

    QString configDirectory = m_baseConfigDirectory + "/" + m_phoneNumber;
    QString configPath = QString(configDirectory).replace("~",QDir::homePath());

    if (!m_pubKey) {
        m_pubKey = Utils::rsaLoadPublicKey(publicKeyFile);
        if(!m_pubKey)
            return false;

        m_pkFingerprint = Utils::computeRSAFingerprint(m_pubKey);
        qCDebug(TG_CORE_SETTINGS) << "loaded Telegram public key from file:" << publicKeyFile;
    }

    qCDebug(TG_CORE_SETTINGS) << "loading configuration from path:" << configPath;
    QDir configDir = QDir(configPath);
    if (!configDir.exists()) {
        if (QDir::root().mkpath(configPath)) {
            qCDebug(TG_CORE_SETTINGS) << configPath << "config directory created";
        } else {
            qCWarning(TG_CORE_SETTINGS) << "error creating config directory: " << configPath;
            return false;
        }
    }

    //check if config file is there and it's readable
    QString configFilename = configPath + '/' + CONFIG_FILE;
    QFile configFile(configFilename);
    if (!configFile.exists()) {
        // config file missing, touch it
        if (configFile.open(QIODevice::WriteOnly)) {
            QTextStream out(&configFile);
            out << DEFAULT_CONFIG_CONTENTS;
            configFile.close();
        } else {
            qCWarning(TG_CORE_SETTINGS) << "Impossible to open file " << configFilename << " for writting default configuration";
            return false;
        }
    }

    //read settings
    qCDebug(TG_CORE_SETTINGS) << "loading settings file" << configFilename;
    QSettings settings(configFilename, QSettings::IniFormat);
    m_testMode = settings.value(ST_TEST_MODE, false).toBool();
    m_managedDownloads = settings.value(ST_MANAGED_DOWNLOADS, false).toBool();
    m_langCode = settings.value(ST_LANG_CODE, "en").toString();
    mResendQueries = settings.value(ST_RESEND_QUERIES, false).toBool();
    m_authFilename = settings.value(ST_AUTH_FILE, configPath + '/' + AUTH_KEY_FILE).toString();
    m_secretChatFilename = settings.value(ST_SECRET, configPath + '/' + SECRET_CHAT_FILE).toString();
    m_stateFilename = settings.value(ST_STATE_FILE, configPath + '/' + STATE_FILE).toString();

    // log readed data
    qCDebug(TG_CORE_SETTINGS) << "testMode:" << m_testMode;
    qCDebug(TG_CORE_SETTINGS) << "managedDownloads:" << m_managedDownloads;
    qCDebug(TG_CORE_SETTINGS) << "lang:" << m_langCode;
    qCDebug(TG_CORE_SETTINGS) << "authFile:" << m_authFilename;
    qCDebug(TG_CORE_SETTINGS) << "secretChatFile:" << m_secretChatFilename;
    qCDebug(TG_CORE_SETTINGS) << "stateFile:" << m_stateFilename;

    readAuthFile();
    readSecretFile();

    return true;
}

void Settings::writeAuthFile() {
    QSettings settings(m_authFilename, QSettings::IniFormat);
    testMode() ? settings.beginGroup(ST_TEST) : settings.beginGroup(ST_PRODUCTION);
    settings.setValue(ST_WORKING_DC_NUM, m_workingDcNum);
    settings.setValue(ST_OUR_ID, m_ourId);
    settings.beginWriteArray(ST_DCS_ARRAY);
    for (qint32 i = 0; i < m_dcsList.length(); i++) {
        //write dc
        settings.setArrayIndex(i);
        settings.setValue(ST_DC_NUM, m_dcsList[i]->id());
        settings.setValue(ST_HOST, m_dcsList[i]->host());
        settings.setValue(ST_PORT, m_dcsList[i]->port());
        settings.setValue(ST_DC_STATE, m_dcsList[i]->state());
        if (m_dcsList[i]->authKeyId()) {
            settings.setValue(ST_AUTH_KEY_ID, m_dcsList[i]->authKeyId());
            QByteArray baToSave(m_dcsList[i]->authKey(), SHARED_KEY_LENGTH);
            settings.setValue(ST_AUTH_KEY, baToSave.toBase64());
        }
        settings.setValue(ST_SERVER_SALT, m_dcsList[i]->serverSalt());
        settings.setValue(ST_EXPIRES, m_dcsList[i]->expires());
    }
    settings.endArray();
    settings.endGroup();
}

void Settings::readAuthFile() {
    QSettings settings(m_authFilename, QSettings::IniFormat);
    testMode() ? settings.beginGroup(ST_TEST) : settings.beginGroup(ST_PRODUCTION);
    qint32 defaultDcId = m_testMode ? TEST_DEFAULT_DC_ID : Settings::defaultHostDcId();
    m_workingDcNum = settings.value(ST_WORKING_DC_NUM, defaultDcId).toInt();
    m_ourId = settings.value(ST_OUR_ID).toInt();
    m_workingDcConfigAvailabe = settings.contains(ST_WORKING_DC_NUM);

    qCDebug(TG_CORE_SETTINGS) << "workingDcNum:" << m_workingDcNum;
    qCDebug(TG_CORE_SETTINGS) << "ourId:" << m_ourId;

    // read all dcs
    m_dcsList.clear();
    qint32 n = settings.beginReadArray(ST_DCS_ARRAY);
    for (qint32 i = 0; i < n; i++) {
        settings.setArrayIndex(i);
        qint32 dcNum = settings.value(ST_DC_NUM).toInt();
        DC* dc = new DC(dcNum);
        dc->setHost(settings.value(ST_HOST).toString());
        dc->setPort(settings.value(ST_PORT, 0).toInt());
        dc->setState((DC::DcState)settings.value(ST_DC_STATE, DC::init).toInt());
        dc->setAuthKeyId(settings.value(ST_AUTH_KEY_ID, 0).toLongLong());
        if (dc->state() >= DC::authKeyCreated) {
            QByteArray readedBa = QByteArray::fromBase64(settings.value(ST_AUTH_KEY).toByteArray());
            memcpy(dc->authKey(), readedBa.data(), SHARED_KEY_LENGTH);
        }
        dc->setServerSalt(settings.value(ST_SERVER_SALT, 0).toLongLong());
        dc->setExpires(settings.value(ST_EXPIRES).toInt());

        qCDebug(TG_CORE_SETTINGS) << "DC | id:" << dc->id() << ", state:" << dc->state() <<
                    ", host:" << dc->host() << ", port:" << dc->port() <<
                    ", expires:" << dc->expires() << ", authKeyId:" << dc->authKeyId() <<
                    ", serverSalt:" << dc->serverSalt();

        m_dcsList.insert(dcNum, dc);
    }
    settings.endArray();
    settings.endGroup();
}

bool Settings::removeAuthFile() {
    QFile authFile(m_authFilename);
    if (authFile.exists()) {
        if (authFile.remove()) {
            qCDebug(TG_CORE_SETTINGS) << "Auth file has been reset";
            return true;
        } else {
            qCDebug(TG_CORE_SETTINGS) << "An error has happened when trying to remove auth file";
        }
    }
    return false;
}

void Settings::readSecretFile() {
    QSettings settings(m_secretChatFilename, QSettings::IniFormat);
    mVersion = settings.value(ST_VERSION, 0).toInt();
    mG = settings.value(ST_G, 0).toInt();
    mP = QByteArray::fromBase64(settings.value(ST_P).toByteArray());

    qCDebug(TG_CORE_SETTINGS) << "secret chats dh version:" << mVersion;
    qCDebug(TG_CORE_SETTINGS) << "secret chats g:" << mG;
    qCDebug(TG_CORE_SETTINGS) << "secret chats p:" << mP.toBase64();

    mSecretChats.clear();
    qint32 n = settings.beginReadArray(ST_SECRET_CHATS_ARRAY);
    for (qint32 i = 0; i < n; i++) {
        settings.setArrayIndex(i);
        SecretChat *secretChat = new SecretChat(this);
        secretChat->setState(SecretChat::Accepted); // only accepted chats are saved.
        secretChat->setChatId(settings.value(ST_CHAT_ID, 0).toInt());
        secretChat->setAccessHash(settings.value(ST_ACCESS_HASH, 0).toLongLong());
        secretChat->setAdminId(settings.value(ST_ADMIN_ID, 0).toInt());
        secretChat->setParticipantId(settings.value(ST_PARTICIPANT_ID, 0).toInt());
        secretChat->setDate(settings.value(ST_DATE, 0).toInt());
        secretChat->setState((SecretChat::State)settings.value(ST_STATE, 0).toInt());

        QByteArray base64Sk = settings.value(ST_SHARED_KEY).toByteArray();
        if (!base64Sk.isEmpty()) {
            QByteArray parsedSk = QByteArray::fromBase64(base64Sk);
            memcpy(secretChat->sharedKey(), parsedSk.data(), SHARED_KEY_LENGTH);
        }

        QByteArray base64Mk = settings.value(ST_MY_KEY).toByteArray();
        if (!base64Mk.isEmpty()) {
            QByteArray parsedMk = QByteArray::fromBase64(base64Mk);
            BIGNUM *myKey = Utils::bytesToBignum(parsedMk);
            secretChat->setMyKey(myKey);
        }

        secretChat->setLayer(settings.value(ST_LAYER, 0).toInt());
        secretChat->setInSeqNo(settings.value(ST_IN_SEQ_NO, 0).toInt());
        secretChat->setOutSeqNo(settings.value(ST_OUT_SEQ_NO, 0).toInt());
        mSecretChats.append(secretChat);

        qCDebug(TG_CORE_SETTINGS) << "SecretChat:\nid:" << secretChat->chatId()
                                  << "\nadminId:" << secretChat->adminId()
                                  << "\nparticipantId:" << secretChat->participantId()
                                  << "\ndate:" << secretChat->date()
                                  << "\nsharedKey (keyFingerprint):" << secretChat->keyFingerprint()
                                  << "\nlayer:" << secretChat->layer()
                                  << "\ninSeqNo:" << secretChat->inSeqNo()
                                  << "\noutSeqNo:" << secretChat->outSeqNo();
    }
    settings.endArray();
}


void Settings::writeSecretFile() {
    QSettings settings(m_secretChatFilename, QSettings::IniFormat);
    settings.clear();
    settings.setValue(ST_VERSION, mVersion);
    settings.setValue(ST_G, mG);
    settings.setValue(ST_P, mP.toBase64());
    settings.beginWriteArray(ST_SECRET_CHATS_ARRAY);
    for (qint32 i = 0; i < mSecretChats.length(); i++) {
        SecretChat *secretChat = mSecretChats[i];
        settings.setArrayIndex(i);
        settings.setValue(ST_CHAT_ID, secretChat->chatId());
        settings.setValue(ST_ACCESS_HASH, secretChat->accessHash());
        settings.setValue(ST_ADMIN_ID, secretChat->adminId());
        settings.setValue(ST_PARTICIPANT_ID, secretChat->participantId());
        settings.setValue(ST_DATE, secretChat->date());
        settings.setValue(ST_STATE, secretChat->state());

        BIGNUM *myKey = secretChat->myKey();
        if (myKey) {
            QByteArray myKeyToSave = Utils::bignumToBytes(myKey);
            settings.setValue(ST_MY_KEY, myKeyToSave.toBase64());
        }

        if (secretChat->sharedKey()) {
            QByteArray sharedKeyToSave((char *)secretChat->sharedKey(), SHARED_KEY_LENGTH);
            settings.setValue(ST_SHARED_KEY, sharedKeyToSave.toBase64());
        }

        settings.setValue(ST_LAYER, secretChat->layer());
        settings.setValue(ST_IN_SEQ_NO, secretChat->inSeqNo());
        settings.setValue(ST_OUT_SEQ_NO, secretChat->outSeqNo());
    }
    settings.endArray();
}

