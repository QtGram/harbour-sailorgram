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

#include "connection.h"
#include "util/constants.h"

#include <QTimer>

#ifdef Q_OS_LINUX
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif

Q_LOGGING_CATEGORY(TG_CORE_CONNECTION, "tg.core.connection")

Connection::Connection(const QString &host, qint32 port, QObject *parent) :
    QTcpSocket(parent),
    Endpoint(host, port),
    mOpLength(0) {
    mBuffer.clear();

    connect(this, SIGNAL(connected()), SLOT(onConnected()));
    connect(this, SIGNAL(disconnected()), SLOT(onDisconnected()));
    connect(this, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(onError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(onStateChanged(QAbstractSocket::SocketState)));

    connect(&mAsserter,SIGNAL(fatalError()), SIGNAL(fatalError()));
}

Connection::~Connection() {
}

void Connection::setupSocket() {
    // See: http://goo.gl/0pjCQo
    // setSocketOption(QAbstractSocket::KeepAliveOption, 1);

#ifdef Q_OS_LINUX
    int enableKeepAlive = 1;
    int fd = socketDescriptor();
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &enableKeepAlive, sizeof(enableKeepAlive));

    int maxIdle = 5; // 5 seconds
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &maxIdle, sizeof(maxIdle));

    int count = 3; // send up to 3 keepalive packets out, then disconnect if no response
    setsockopt(fd, SOL_TCP, TCP_KEEPCNT, &count, sizeof(count));

    int interval = 2; // send a keepalive packet out every 2 seconds (after the first idle period)
    setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
#endif
}

qint64 Connection::writeOut(const void *data, qint64 length){
    if (state() != QAbstractSocket::ConnectedState) { return -1; }
    if (!length) { return 0; }
    Q_ASSERT(length > 0);
    return write((const char *)data, length);
}

qint32 Connection::peekIn(void *data, qint32 len) {
    if (!len || bytesAvailable() < 1) { return 0; }
    Q_ASSERT(len > 0);
    if (len > bytesAvailable()) {
        len = bytesAvailable();
    }
    return peek((char *)data, len);
}

qint32 Connection::readIn(void *data, qint32 len) {
    if (!len || bytesAvailable() < 1) { return 0; }
    Q_ASSERT(len > 0);
    if (len > bytesAvailable()) {
        len = bytesAvailable();
    }
    return read((char *)data, len);
}

QByteArray Connection::readIn(qint32 len) {
    if (!len || bytesAvailable() < 1) { return 0; }
    Q_ASSERT(len > 0);
    if (len > bytesAvailable()) {
        len = bytesAvailable();
    }
    return read(len);
}

QByteArray Connection::readAll() {
    if (bytesAvailable() < 1) { return 0; }
    return readAll();
}

void Connection::connectToServer() {
    Q_ASSERT(!m_host.isEmpty());
    Q_ASSERT(m_port);
    connectToHost(m_host, m_port);
}

void Connection::onStateChanged(QAbstractSocket::SocketState state) {
    qCDebug(TG_CORE_CONNECTION) << "Socket state changed to " << state;
}

/*
QAbstractSocket::ConnectionRefusedError             0	The connection was refused by the peer (or timed out).
QAbstractSocket::RemoteHostClosedError              1	The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.
QAbstractSocket::HostNotFoundError                  2	The host address was not found.
QAbstractSocket::SocketAccessError                  3	The socket operation failed because the application lacked the required privileges.
QAbstractSocket::SocketResourceError                4	The local system ran out of resources (e.g., too many sockets).
QAbstractSocket::SocketTimeoutError                 5	The socket operation timed out.
QAbstractSocket::DatagramTooLargeError              6	The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).
QAbstractSocket::NetworkError                       7	An error occurred with the network (e.g., the network cable was accidentally plugged out).
QAbstractSocket::AddressInUseError                  8	The address specified to QAbstractSocket::bind() is already in use and was set to be exclusive.
QAbstractSocket::SocketAddressNotAvailableError     9	The address specified to QAbstractSocket::bind() does not belong to the host.
QAbstractSocket::UnsupportedSocketOperationError	10	The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support).
QAbstractSocket::ProxyAuthenticationRequiredError	12	The socket is using a proxy, and the proxy requires authentication.
QAbstractSocket::SslHandshakeFailedError            13	The SSL/TLS handshake failed, so the connection was closed (only used in QSslSocket)
QAbstractSocket::UnfinishedSocketOperationError     11	Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background).
QAbstractSocket::ProxyConnectionRefusedError        14	Could not contact the proxy server because the connection to that server was denied
QAbstractSocket::ProxyConnectionClosedError         15	The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)
QAbstractSocket::ProxyConnectionTimeoutError        16	The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase.
QAbstractSocket::ProxyNotFoundError                 17	The proxy address set with setProxy() (or the application proxy) was not found.
QAbstractSocket::ProxyProtocolError                 18	The connection negotiation with the proxy server failed, because the response from the proxy server could not be understood.
QAbstractSocket::OperationError                     19	An operation was attempted while the socket was in a state that did not permit it.
QAbstractSocket::SslInternalError                   20	The SSL library being used reported an internal error. This is probably the result of a bad installation or misconfiguration of the library.
QAbstractSocket::SslInvalidUserDataError            21	Invalid data (certificate, key, cypher, etc.) was provided and its use resulted in an error in the SSL library.
QAbstractSocket::TemporaryError                     22	A temporary error occurred (e.g., operation would block and socket is non-blocking).
QAbstractSocket::UnknownSocketError                 -1	An unidentified error occurred.
*/
void Connection::onError(QAbstractSocket::SocketError error) {
    qCWarning(TG_CORE_CONNECTION) << "SocketError:" << QString::number(error) << errorString();
    if (error <= QAbstractSocket::NetworkError) {
        if (state() == QAbstractSocket::ConnectedState || state() == QAbstractSocket::ConnectingState) {
            disconnectFromHost();
        }

        qint32 reconnectionDelay = 0;
        if (errorString() == "Network unreachable") {
            // In this case, there is no way to reach the server because the physical link
            // is broken (disconnected all connections). Don't retry reconnecting continously
            // and insert a delay between reconnection attempts.
            reconnectionDelay = 5000;
        }
        // From http://doc.qt.io/qt-5/qabstractsocket.html#error
        // "When this signal is emitted, the socket may not be ready for a reconnect attempt."
        // Let's wait for the event loop spin once
        QTimer::singleShot(reconnectionDelay, this, SLOT(connectToServer()));
    }
}

void Connection::onConnected() {
    // stop trying reconnect if it was alive
    setupSocket();

    // abridged version of the protocol requires sending 0xef byte at beginning
    unsigned char byte = 0xef;
    qint32 writtenBytes = writeOut(&byte, 1);
    Q_UNUSED(writtenBytes);
    Q_ASSERT(writtenBytes == 1);

    // process the rest of operations in inherited classes
    processConnected();
}

void Connection::onDisconnected() {
    qCWarning(TG_CORE_CONNECTION) << "Socket disconnected";
}

void Connection::onReadyRead() {

    while (bytesAvailable()) {
        if (!mOpLength) {
            // calculate length, read first byte
            qint32 readed = readIn(&mOpLength, 1);
            if (mOpLength == 0x7f) {
                readed = readIn(&mOpLength, 3);
            }
            Q_UNUSED(readed)
            mOpLength *= 4;
        }

        QByteArray buffer = readIn(mOpLength - mBuffer.length());

        qint32 opReaded = buffer.length() + mBuffer.length();

        if (opReaded == mOpLength) {
            //process request
            mBuffer.append(buffer);
            QMetaObject::invokeMethod(this, "processRpcAnswer", Qt::QueuedConnection, Q_ARG(QByteArray, mBuffer));
            mOpLength = 0;
            mBuffer.clear();
        } else if (opReaded < mOpLength) {
            mBuffer.append(buffer);
        }
    }
}
