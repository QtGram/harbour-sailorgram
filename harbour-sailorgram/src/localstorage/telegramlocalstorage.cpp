#include "telegramlocalstorage.h"

const QString TelegramLocalStorage::TELEGRAM_CONFIG = "telegram";
const QString TelegramLocalStorage::TELEGRAM_PUBLIC_KEY = "server.pub";

TelegramLocalStorage::TelegramLocalStorage(QObject *parent): QObject(parent)
{
    QDir cfgdir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    cfgdir.mkpath(qApp->applicationName() + QDir::separator() + qApp->applicationName() + QDir::separator() + TelegramLocalStorage::TELEGRAM_CONFIG);
}

QString TelegramLocalStorage::telegramConfigPath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QDir::separator() + TelegramLocalStorage::TELEGRAM_CONFIG;
}

QString TelegramLocalStorage::telegramPublicKey() const
{
    return qApp->applicationDirPath() + QDir::separator() + "../share/" + qApp->applicationName() + QDir::separator() + TelegramLocalStorage::TELEGRAM_PUBLIC_KEY;
}
