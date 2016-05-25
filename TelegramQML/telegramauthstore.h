#ifndef TELEGRAMAUTHSTORE_H
#define TELEGRAMAUTHSTORE_H

#include <QObject>
#include <QJSValue>

#include "telegramqml_macros.h"
#include "telegramqml_global.h"
#include "tqbaseobject.h"

class TelegramAuthStorePrivate;
class TelegramAuthStore : public QObject, public TqBaseObject
{
    Q_OBJECT
    Q_PROPERTY(QJSValue writeMethod READ writeMethod WRITE setWriteMethod NOTIFY writeMethodChanged)
    Q_PROPERTY(QJSValue readMethod READ readMethod WRITE setReadMethod NOTIFY readMethodChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)

public:
    TelegramAuthStore(QObject *parent = 0);
    ~TelegramAuthStore();

    void setWriteMethod(const QJSValue &method);
    QJSValue writeMethod() const;

    void setReadMethod(const QJSValue &method);
    QJSValue readMethod() const;

    bool isValid() const;
    static QStringList requiredProperties();

Q_SIGNALS:
    void writeMethodChanged();
    void readMethodChanged();
    void isValidChanged();

private:
    TelegramAuthStorePrivate *p;
};

#endif // TELEGRAMAUTHSTORE_H
