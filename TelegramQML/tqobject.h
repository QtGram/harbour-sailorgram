#ifndef TQOBJECT_H
#define TQOBJECT_H

#include <QObject>
#include <QQmlListProperty>

#include "telegramtools.h"
#include "telegramqml_global.h"
#include "tqbaseobject.h"

#define tqobject_cast(OBJECT) static_cast<TqObject*>(OBJECT)

class TELEGRAMQMLSHARED_EXPORT TqObject : public QObject, public TqBaseObject
{
    Q_OBJECT
    Q_PROPERTY(QString errorText READ errorText NOTIFY errorChanged)
    Q_PROPERTY(qint32 errorCode READ errorCode NOTIFY errorChanged)
    Q_PROPERTY(QQmlListProperty<QObject> items READ items NOTIFY itemsChanged)
    Q_CLASSINFO("DefaultProperty", "items")

public:
    Q_INVOKABLE explicit TqObject(QObject *parent = 0);
    ~TqObject();

    static bool isValid(TqObject *obj);

    QString errorText() const { return mErrorText; }
    qint32 errorCode() const { return mErrorCode; }

    QQmlListProperty<QObject> items();
    static QStringList requiredProperties() {
        return QStringList();
    }

Q_SIGNALS:
    void errorChanged();
    void itemsChanged();

protected:
    void setError(const QString &errorText, qint32 errorCode) {
        mErrorText = TelegramTools::convertErrorToText(errorText);
        mErrorCode = errorCode;
        Q_EMIT errorChanged();
    }

private:
    static void append(QQmlListProperty<QObject> *p, QObject *v);
    static int count(QQmlListProperty<QObject> *p);
    static QObject *at(QQmlListProperty<QObject> *p, int idx);
    static void clear(QQmlListProperty<QObject> *p);

protected:
    QList<QObject*> _items;

private:
    QString mErrorText;
    qint32 mErrorCode;
};

#endif // TQOBJECT_H
