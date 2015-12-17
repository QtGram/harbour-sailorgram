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

#ifndef CONNECTION_H
#define CONNECTION_H

#define MAX_RESPONSE_SIZE (1L << 24)


#include <QtNetwork/QTcpSocket>
#include "endpoint.h"
#include <QLoggingCategory>
#include "util/constants.h"
#include "util/asserter.h"

Q_DECLARE_LOGGING_CATEGORY(TG_CORE_CONNECTION)

class Connection : public QTcpSocket, public Endpoint
{
    Q_OBJECT
public:
    explicit Connection(const QString &host = QString::null, qint32 port = -1, QObject *parent = 0);
    virtual ~Connection();

    void connectToServer();

Q_SIGNALS:
    void fatalError();

protected:
    void setupSocket();
    qint64 writeOut(const void *data, qint64 length);
    QByteArray readAll();
    QByteArray readIn(qint32 len);
    qint32 readIn(void *data, qint32 len);
    qint32 peekIn(void *data, qint32 len);
    virtual void processRpcAnswer(QByteArray response) = 0;
    virtual void processConnected() = 0;

    void timerEvent(QTimerEvent *event);

    Asserter mAsserter;

private:
    qint32 mReconnectTimerId;
    QByteArray mBuffer;
    qint32 mOpLength;

    void stopReconnecting();

protected Q_SLOTS:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError error);
};

#endif // CONNECTION_H
