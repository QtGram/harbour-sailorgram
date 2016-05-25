#ifndef TELEGRAMABSTRACTLISTMODEL_H
#define TELEGRAMABSTRACTLISTMODEL_H

#include "telegramqml_global.h"
#include "telegramtools.h"
#include "tqbaseobject.h"

#include <QAbstractListModel>
#include <QQmlListProperty>

class TELEGRAMQMLSHARED_EXPORT TelegramAbstractListModel : public QAbstractListModel, public TqBaseObject
{
    Q_OBJECT
    Q_PROPERTY(int count   READ count   NOTIFY countChanged)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY isEmptyChanged)
    Q_PROPERTY(QString errorText READ errorText NOTIFY errorChanged)
    Q_PROPERTY(qint32 errorCode READ errorCode NOTIFY errorChanged)
    Q_PROPERTY(QQmlListProperty<QObject> items READ items NOTIFY itemsChanged)
    Q_CLASSINFO("DefaultProperty", "items")

public:
    TelegramAbstractListModel(QObject *parent = 0);
    ~TelegramAbstractListModel();

    Q_INVOKABLE QStringList roles() const;

    int rowCount(const QModelIndex &) const { return count(); }
    virtual int count() const = 0;
    bool isEmpty() const { return count() == 0; }

    QString errorText() const { return mErrorText; }
    qint32 errorCode() const { return mErrorCode; }

    QQmlListProperty<QObject> items();
    static QStringList requiredProperties() {
        return QStringList();
    }

public Q_SLOTS:
    QVariant get(int index, int role) const;
    QVariant get(int index, const QString &roleName) const;
    QVariantMap get(int index) const;
    int indexOf(int role, const QVariant &value);

Q_SIGNALS:
    void countChanged();
    void errorChanged();
    void itemsChanged();
    void isEmptyChanged();

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
    int _cacheCount;
};

#endif // TELEGRAMABSTRACTLISTMODEL_H
