#ifndef TELEGRAMLOCALSTORAGE_H
#define TELEGRAMLOCALSTORAGE_H

#include <QObject>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

class TelegramLocalStorage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString telegramConfigPath READ telegramConfigPath CONSTANT FINAL)
    Q_PROPERTY(QString telegramPublicKey READ telegramPublicKey CONSTANT FINAL)

    public:
        explicit TelegramLocalStorage(QObject *parent = 0);
        QString telegramConfigPath() const;
        QString telegramPublicKey() const;

    signals:

    public slots:

    private:
        static const QString TELEGRAM_CONFIG;
        static const QString TELEGRAM_PUBLIC_KEY;
};

#endif // TELEGRAMLOCALSTORAGE_H
