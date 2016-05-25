#include "telegramapp.h"

class TelegramAppPrivate
{
public:
    qint32 appId;
    QString appHash;
    bool valid;
};

TelegramApp::TelegramApp(QObject *parent) :
    QObject(parent)
{
    p = new TelegramAppPrivate;
    p->valid = false;
    p->appId = 0;
}

void TelegramApp::setAppId(const qint32 &appId)
{
    if(p->appId == appId)
        return;

    p->appId = appId;
    Q_EMIT appIdChanged();
    refreshValid();
}

qint32 TelegramApp::appId() const
{
    return p->appId;
}

void TelegramApp::setAppHash(const QString &appHash)
{
    if(p->appHash == appHash)
        return;

    p->appHash = appHash;
    Q_EMIT appHashChanged();
    refreshValid();
}

QString TelegramApp::appHash() const
{
    return p->appHash;
}

bool TelegramApp::isValid() const
{
    return p->appId && !p->appHash.isEmpty();
}

QStringList TelegramApp::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(appHash) << FUNCTION_NAME(appId);
}

void TelegramApp::refreshValid()
{
    const bool valid = isValid();
    if(valid == p->valid)
        return;

    p->valid = valid;
    Q_EMIT isValidChanged();
}

TelegramApp::~TelegramApp()
{
    p->appHash.clear();
    refreshValid();
    delete p;
}

