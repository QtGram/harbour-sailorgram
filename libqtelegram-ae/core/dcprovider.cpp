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

#include "dcprovider.h"
#include "util/constants.h"
#include "session.h"

#include <QDebug>

Q_LOGGING_CATEGORY(TG_CORE_DCPROVIDER, "tg.core.dcprovider")

DcProvider::DcProvider(Settings *settings, CryptoUtils *crypto) :
    mSettings(settings),
    mCrypto(crypto),
    mApi(0),
    mPendingDcs(0),
    mPendingTransferSessions(0),
    mWorkingDcSession(0) {
}

DcProvider::~DcProvider() {
   clean();
}

void DcProvider::clean() {
    Q_FOREACH (DC *dc, mDcs) {
        delete dc;
        dc = 0;
    }
    Q_FOREACH (DCAuth *dcAuth, mDcAuths) {
        if (dcAuth) {
            if (dcAuth->state() != QAbstractSocket::UnconnectedState) {
                connect(dcAuth, SIGNAL(disconnected()), dcAuth, SLOT(deleteLater()));
                dcAuth->disconnectFromHost();
            } else {
                dcAuth->deleteLater();
            }
            dcAuth = 0;
        }
    }
    Q_FOREACH (Session *session, mTransferSessions)  {
        if (session) {
            session->close();
        }
    }
    if (mApi) {
        mApi->deleteLater();
        mApi = 0;
    }
    mDcs.clear();
    mDcAuths.clear();
    mTransferSessions.clear();

    mPendingDcs = 0;
    mPendingTransferSessions = 0;
    mWorkingDcSession = 0;
}

DC *DcProvider::getDc(qint32 dcNum) const {
    return mDcs.value(dcNum, NULL);
}

DC *DcProvider::getWorkingDc() const {
    return mDcs.value(mSettings->workingDcNum(), 0);
}


void DcProvider::initialize() {

    clean();

    /**
      1.- Read settings (dcsList and working dc num)
      2.- Fullfill m_dcs map with dcsList
      3.- Connect to working dc
      4.- Ask for config when working dc is connected
      5.- Connect to the rest of the dcs received in config
      */

    // 1.- Get the settings dcs, fullfill m_dcs map and current dc num
    QList<DC *> dcsList = mSettings->dcsList();

    Q_FOREACH (DC *dc, dcsList) {
        mDcs.insert(dc->id(), dc);
    }

    qint32 defaultDcId = mSettings->testMode() ? TEST_DEFAULT_DC_ID : mSettings->defaultHostDcId();

    // 2.- connect to working DC
    if (!mSettings->workingDcConfigAvailabe()) {
        // If dcId == defaultDcId, it's the default one, so call default host and port
        if (!mDcs.value(defaultDcId, 0)) {
            mDcs[defaultDcId] = new DC(defaultDcId);
        }
        // if dc hasn't key created, create it
        if (mDcs[defaultDcId]->state() < DC::authKeyCreated) {
            QString defaultDcHost = mSettings->defaultHostAddress();
            qint32 defaultDcPort = mSettings->defaultHostPort();
            if (mSettings->testMode()) {
                defaultDcHost = TEST_DEFAULT_DC_HOST;
                defaultDcPort = TEST_DEFAULT_DC_PORT;
            }

            // create a dc authenticator based in dc info
            mDcs[defaultDcId]->setHost(defaultDcHost);
            mDcs[defaultDcId]->setPort(defaultDcPort);
            DCAuth *dcAuth = new DCAuth(mDcs[defaultDcId], mSettings, mCrypto, this);
            mDcAuths.insert(defaultDcId, dcAuth);
            connect(dcAuth, SIGNAL(fatalError()), this, SLOT(logOut()));
            connect(dcAuth, SIGNAL(fatalError()), this, SIGNAL(fatalError()));
            connect(dcAuth, SIGNAL(dcReady(DC*)), this, SLOT(onDcReady(DC*)));
            dcAuth->createAuthKey();
        } else {
            onDcReady(mDcs[defaultDcId]);
        }

    } else {
        // In any other case, the host and port have been retrieved from auth file settings and the DC object is already created
        if (mDcs[mSettings->workingDcNum()]->state() < DC::authKeyCreated) {
            // create a dc authenticator based in dc info
            DCAuth *dcAuth = new DCAuth(mDcs[mSettings->workingDcNum()], mSettings, mCrypto);
            mDcAuths.insert(mSettings->workingDcNum(), dcAuth);
            connect(dcAuth, SIGNAL(fatalError()), this, SLOT(logOut()));
            connect(dcAuth, SIGNAL(fatalError()), this, SIGNAL(fatalError()));
            connect(dcAuth, SIGNAL(dcReady(DC*)), this, SLOT(onDcReady(DC*)));
            dcAuth->createAuthKey();
        } else {
            onDcReady(mDcs[mSettings->workingDcNum()]);
        }
    }
}

void DcProvider::onDcReady(DC *dc) {
    qCDebug(TG_CORE_DCPROVIDER) << "DC" << dc->id() << "ready";
    // disconnect dcAuth to avoid consuming bandwidth with unnecessary connections
    DCAuth *dcAuth = mDcAuths.value(dc->id());
    if (dcAuth) {
        if (dcAuth->state() != QAbstractSocket::UnconnectedState) {
            connect(dcAuth, SIGNAL(disconnected()), this, SLOT(onDcAuthDisconnected()));
            dcAuth->disconnectFromHost();
        } else {
            processDcReady(dc);
        }
    } else {
        processDcReady(dc);
    }
}

void DcProvider::onDcAuthDisconnected() {
    DCAuth *dcAuth = qobject_cast<DCAuth *>(sender());
    processDcReady(dcAuth->dc());
    mDcAuths.remove(dcAuth->dc()->id());
    dcAuth->deleteLater();
}

void DcProvider::processDcReady(DC *dc) {
    // create api object if dc is workingDc, and get configuration
    if ((!mApi) && (dc->id() == mSettings->workingDcNum())) {
        Session *session = new Session(dc, mSettings, mCrypto, this);
        mApi = new Api(session, mSettings, mCrypto, this);
        connect(session, SIGNAL(sessionReady(DC*)), this, SLOT(onApiReady(DC*)));
        connect(session, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onApiError()));
        session->connectToServer();
    } else if (--mPendingDcs == 0) { // if all dcs are authorized, emit provider ready signal
        // save the settings here, after all dcs are ready
        mSettings->setDcsList(mDcs.values());
        mSettings->writeAuthFile();

        qCDebug(TG_CORE_DCPROVIDER) << "DcProvider ready";
        Q_EMIT dcProviderReady();

        // if current dc is in "authKeyCreated" state, authNeeded signal must be emitted for user to sign in.
        // if current dc is in "userSignedIn" state, check that all dcs are in that state or export/import auth.
        // If they aren't, transfer auth from workingDc to workingDc+1, from workingDc+1 to workingDc+2...etc until completed all.
        switch (mDcs.value(mSettings->workingDcNum())->state()) {
        case DC::authKeyCreated:
            Q_EMIT authNeeded();
            break;
        case DC::userSignedIn:
            transferAuth();
            break;
        default:
//            Q_ASSERT(0); // it's impossible getting here. This is done to avoid warning when compilation
            break;
        }

    }
}

void DcProvider::onApiError() {

    Session *session = qobject_cast<Session *>(sender());
    qCWarning(TG_CORE_DCPROVIDER) << "Api init error when connecting session to server:" << session->errorString();

    // after emitting these startup offline signals, we don't want to do it again when session gets connected, so disconnect signal-slot
    disconnect(session, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onApiError()));
    // also disconnect the signal-slot to query for current server config when connected (once logged in that info is irrelevant)
    disconnect(session, SIGNAL(sessionReady(DC*)), this, SLOT(onApiReady(DC*)));

    bool userSignedInAllDCs = true;
    // because we haven't connection, we assume the available DCs have not changed since last startup.
    // Check if there is a shared key created for all DCs readed from config and if the user has
    // been authenticated in all them
    Q_FOREACH (DC *dc, mDcs) {
        if (dc->state() < DC::authKeyCreated) {
            // emit a fatal error that should be shown in app as the reason to abort starting up the app
            Q_EMIT error(-1,
                         500,
                         "Impossible to start application. There is no connection to server and stored credentials are not valid");
            return;
        } else if (dc->state() < DC::userSignedIn) {
            userSignedInAllDCs = false;
        }
    }

    Q_EMIT dcProviderReady();

    if (userSignedInAllDCs) {
        Q_EMIT authTransferCompleted();
    } else {
        Q_EMIT authNeeded();
    }
}

void DcProvider::onApiReady(DC*) {
    Session *session = qobject_cast<Session *>(sender());
    qCDebug(TG_CORE_DCPROVIDER) << "Api connected to server and ready";

    // after emitting the api startup signals, we don't want to do it again when session gets connected, so disconnect signal-slot
    disconnect(session, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onApiError()));
    // disconnect the signal-slot to query for current server config when connected (once logged in that info is irrelevant)
    disconnect(session, SIGNAL(sessionReady(DC*)), this, SLOT(onApiReady(DC*)));

    // get the config
    connect(mApi, SIGNAL(config(qint64,const Config&)), this, SLOT(onConfigReceived(qint64,const Config&)));
    mApi->helpGetConfig();
}

void DcProvider::onConfigReceived(qint64 msgId, const Config &config) {

    qCDebug(TG_CORE_DCPROVIDER) << "onConfigReceived(), msgId =" << QString::number(msgId, 16);
    qCDebug(TG_CORE_DCPROVIDER) << "date =" << config.date();
    qCDebug(TG_CORE_DCPROVIDER) << "testMode =" << config.testMode();
    qCDebug(TG_CORE_DCPROVIDER) << "thisDc =" << config.thisDc();

    const QList<DcOption> &dcOptions = config.dcOptions();

    mPendingDcs = dcOptions.length() -1; //all the received options but the default one, yet used

    Q_FOREACH (DcOption dcOption, dcOptions) {
        qCDebug(TG_CORE_DCPROVIDER) << "dcOption | id =" << dcOption.id() << ", ipAddress =" << dcOption.ipAddress() <<
                    ", port =" << dcOption.port() << ", hostname =" << dcOption.hostname();

        // for every new DC or not authenticated DC, insert into m_dcs and authenticate
        DC *dc = mDcs.value(dcOption.id());

        // check if dc is not null or if received host and port are not equals than settings ones
        if ((!dc) || ((dc->host() != dcOption.ipAddress()) || (dc->port() != dcOption.port()))) {
            // if not exists dc or host and port different, create a new dc object for this dcId and add it to m_dcs map
            dc = new DC(dcOption.id());
            dc->setHost(dcOption.ipAddress());
            dc->setPort(dcOption.port());
            mDcs.insert(dcOption.id(), dc);
        }

        // let's see if needed to create shared key for it
        // In any other case, the host and port have been retrieved from auth file settings and the DC object is already created
        if (dc->state() < DC::authKeyCreated) {
            // create a dc authenticator based in dc info
            DCAuth *dcAuth = new DCAuth(dc, mSettings, mCrypto, this);
            mDcAuths.insert(dcOption.id(), dcAuth);
            connect(dcAuth, SIGNAL(fatalError()), this, SLOT(logOut()));
            connect(dcAuth, SIGNAL(fatalError()), this, SIGNAL(fatalError()));
            connect(dcAuth, SIGNAL(dcReady(DC*)), this, SLOT(onDcReady(DC*)));
            dcAuth->createAuthKey();
        } else if (dcOption.id() != config.thisDc()) {
            // if authorized and not working dc emit dcReady signal directly
            onDcReady(dc);
        }
    }

    qCDebug(TG_CORE_DCPROVIDER) << "chatMaxSize =" << config.chatSizeMax();
    qCDebug(TG_CORE_DCPROVIDER) << "broadcastMaxSize =" << config.broadcastSizeMax();
}

Api *DcProvider::getApi() const {
    return mApi;
}

QList<DC *> DcProvider::getDcs() const {
    return mDcs.values();
}

void DcProvider::transferAuth() {
    // add in a list all the dcs in DcState::authKeyCreated but not in DCState::userSignedIn
    // and receiving the authorization data (all but working dc one)
    Q_ASSERT(mApi);
    bool hasTransferSessions = false;
    mWorkingDcSession = mApi->mainSession();
    Q_ASSERT(mWorkingDcSession);
    QList<DC *> m_dcsList = mDcs.values();
    for (qint32 i=0 ; i < m_dcsList.size(); i++) {
        DC *dc = m_dcsList.value(i);
        if (dc->state() == DC::authKeyCreated &&
                dc->id() != mSettings->workingDcNum()) {
            hasTransferSessions = true;
            // create a new session for this dc
            Session *session = mApi->fileSession(dc);
            connect(session, SIGNAL(sessionReady(DC*)), this, SLOT(onTransferSessionReady(DC*)));
            session->connectToServer();
            mPendingTransferSessions++;
        }
    }
    if (!hasTransferSessions) {
        Q_EMIT authTransferCompleted();
    }
}

void DcProvider::onTransferSessionReady(DC *) {
    Session *session = qobject_cast<Session *>(sender());
    mTransferSessions.append(session);
    if (--mPendingTransferSessions == 0) {
        connect(mApi, SIGNAL(authExportedAuthorization(qint64,qint32,const QByteArray&)), this, SLOT(onAuthExportedAuthorization(qint64,qint32,const QByteArray&)));
        connect(mApi, SIGNAL(authImportedAuthorization(qint64,qint32,const User&)), this, SLOT(onAuthImportedAuthorization(qint64,qint32,const User&)));
        mApi->authExportAuthorization(mTransferSessions.first()->dc()->id());
    }
}

void DcProvider::onAuthExportedAuthorization(qint64, qint32 ourId, const QByteArray &bytes) {
    // Set ourId into settings (It doesn't matter if set before)
    mSettings->setOurId(ourId);
    // Change api dc to first in the transfer dcs list
    mApi->setMainSession(mTransferSessions.first());
    // Execute import in this dc
    mApi->authImportAuthorization(ourId, bytes);
}

void DcProvider::onAuthImportedAuthorization(qint64, qint32 expires, const User &) {
    Session *session = mTransferSessions.takeFirst();
    DC *authorizedDc = session->dc();
    authorizedDc->setExpires(expires);
    authorizedDc->setState(DC::userSignedIn);
    session->release();
    mApi->setMainSession(mWorkingDcSession);
    if (mTransferSessions.isEmpty()) {
        mSettings->setDcsList(mDcs.values());
        mSettings->writeAuthFile();
        Q_EMIT authTransferCompleted();
    } else {
        mApi->authExportAuthorization(mTransferSessions.first()->dc()->id());
    }
}

void DcProvider::logOut() {
    Q_FOREACH (qint32 dcId, mDcs.keys()) {
        DC *dc = mDcs.value(dcId);
        dc->setState(DC::init);
    }
    mSettings->setDcsList(mDcs.values());
    mSettings->writeAuthFile();
}
