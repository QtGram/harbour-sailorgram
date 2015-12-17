#ifndef DATABASEABSTRACTENCRYPTOR_H
#define DATABASEABSTRACTENCRYPTOR_H

#include <QString>
#include <QByteArray>
#include <QVariant>
#include <QObject>

#include "telegramqml_global.h"

class TELEGRAMQMLSHARED_EXPORT DatabaseAbstractEncryptor
{
public:
    DatabaseAbstractEncryptor();
    virtual ~DatabaseAbstractEncryptor();

    virtual QVariant encrypt(const QString &text, bool encryptedMessage) = 0; // Thread safe function
    virtual QString decrypt(const QVariant &data) = 0; // Thread safe function
};

Q_DECLARE_METATYPE(DatabaseAbstractEncryptor*)

#endif // DATABASEABSTRACTENCRYPTOR_H
