#ifndef TELEGRAMQMLSHAREDPOINTER_H
#define TELEGRAMQMLSHAREDPOINTER_H

#include <QObject>

#include "telegramsharedpointer.h"

class TelegramQmlSharedPointer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* object READ object WRITE setObject NOTIFY objectChanged)

public:
    TelegramQmlSharedPointer(QObject *parent = 0);
    ~TelegramQmlSharedPointer();

    void setObject(QObject *object);
    QObject *object() const;

    static QStringList requiredProperties();

public Q_SLOTS:
    void clean();

Q_SIGNALS:
    void objectChanged();

private:
    TelegramSharedPointer<QObject> mObject;
};

#endif // TELEGRAMQMLSHAREDPOINTER_H
