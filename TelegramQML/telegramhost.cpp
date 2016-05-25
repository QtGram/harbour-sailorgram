#include "telegramhost.h"

#include <QDebug>

class TelegramHostPrivate
{
public:
    QString hostAddress;
    qint32 hostPort;
    qint32 hostDcId;
    QUrl publicKey;
    bool valid;
};

TelegramHost::TelegramHost(QObject *parent) :
    QObject(parent)
{
    p = new TelegramHostPrivate;
    p->hostPort = 0;
    p->hostDcId = 0;
    p->valid = false;
    p->publicKey = QUrl("qrc:/files/tg-server.pub");
}

QString TelegramHost::hostAddress() const
{
    return p->hostAddress;
}

void TelegramHost::setHostAddress(const QString &hostAddress)
{
    if(p->hostAddress == hostAddress)
        return;

    p->hostAddress = hostAddress;
    Q_EMIT hostAddressChanged();
    refreshValid();
}

qint32 TelegramHost::hostPort() const
{
    return p->hostPort;
}

void TelegramHost::setHostPort(qint32 hostPort)
{
    if(p->hostPort == hostPort)
        return;

    p->hostPort = hostPort;
    Q_EMIT hostPortChanged();
    refreshValid();
}

qint32 TelegramHost::hostDcId() const
{
    return p->hostDcId;
}

void TelegramHost::setHostDcId(qint32 hostDcId)
{
    if(p->hostDcId == hostDcId)
        return;

    p->hostDcId = hostDcId;
    Q_EMIT hostDcIdChanged();
    refreshValid();
}

QUrl TelegramHost::publicKey() const
{
    return p->publicKey;
}

void TelegramHost::setPublicKey(const QUrl &publicKey)
{
    if(p->publicKey == publicKey)
        return;

    p->publicKey = publicKey;
    if(p->publicKey.isValid() && !p->publicKey.isLocalFile())
        qDebug() << "Error: The public key is not a local file.";

    Q_EMIT publicKeyChanged();
    refreshValid();
}

bool TelegramHost::isValid() const
{
    return p->hostDcId && p->hostPort &&
           !p->hostAddress.isEmpty() &&
            p->publicKey.isValid();
}

QStringList TelegramHost::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(hostDcId)
                         << FUNCTION_NAME(hostPort)
                         << FUNCTION_NAME(hostAddress)
                         << FUNCTION_NAME(publicKey);
}

void TelegramHost::refreshValid()
{
    const bool valid = isValid();
    if(valid == p->valid)
        return;

    p->valid = valid;
    Q_EMIT isValidChanged();
}

TelegramHost::~TelegramHost()
{
    p->hostAddress.clear();
    refreshValid();
    delete p;
}

