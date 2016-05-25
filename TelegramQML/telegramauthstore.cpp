#include "telegramauthstore.h"

class TelegramAuthStorePrivate
{
public:
    QJSValue writeMethod;
    QJSValue readMethod;
};

TelegramAuthStore::TelegramAuthStore(QObject *parent) :
    QObject(parent)
{
    p = new TelegramAuthStorePrivate;
}

void TelegramAuthStore::setWriteMethod(const QJSValue &method)
{
    if(p->writeMethod.isNull() && method.isNull())
        return;

    p->writeMethod = method;
    Q_EMIT writeMethodChanged();
    Q_EMIT isValidChanged();
}

QJSValue TelegramAuthStore::writeMethod() const
{
    return p->writeMethod;
}

void TelegramAuthStore::setReadMethod(const QJSValue &method)
{
    if(p->readMethod.isNull() && method.isNull())
        return;

    p->readMethod = method;
    Q_EMIT readMethodChanged();
    Q_EMIT isValidChanged();
}

QJSValue TelegramAuthStore::readMethod() const
{
    return p->readMethod;
}

bool TelegramAuthStore::isValid() const
{
    return !p->readMethod.isNull() && !p->writeMethod.isNull() &&
            p->readMethod.isCallable() && p->writeMethod.isCallable();
}

QStringList TelegramAuthStore::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(readMethod)
                         << FUNCTION_NAME(writeMethod);
}

TelegramAuthStore::~TelegramAuthStore()
{
    delete p;
}
