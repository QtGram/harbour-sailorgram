#ifndef TQOBJECT_H
#define TQOBJECT_H

#include <QObject>
#include "telegramqml_global.h"

#define tqobject_cast(OBJECT) static_cast<TqObject*>(OBJECT)

class TELEGRAMQMLSHARED_EXPORT TqObject : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit TqObject(QObject *parent = 0);
    ~TqObject();

    static bool isValid(TqObject *obj);
};

#endif // TQOBJECT_H
