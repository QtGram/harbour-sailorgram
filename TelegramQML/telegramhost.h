#ifndef TELEGRAMHOST_H
#define TELEGRAMHOST_H

#include <QObject>
#include <QUrl>

#include "telegramqml_macros.h"
#include "telegramqml_global.h"
#include "tqbaseobject.h"

/*!
 * Define and Keep Telegram's default host details
 * to use in the Telegram.Engine component.
 */
class TelegramHostPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramHost : public QObject, public TqBaseObject
{
    Q_OBJECT
    Q_PROPERTY(QString hostAddress READ hostAddress WRITE setHostAddress NOTIFY hostAddressChanged)
    Q_PROPERTY(qint32 hostPort READ hostPort WRITE setHostPort NOTIFY hostPortChanged)
    Q_PROPERTY(qint32 hostDcId READ hostDcId WRITE setHostDcId NOTIFY hostDcIdChanged)
    Q_PROPERTY(QUrl publicKey READ publicKey WRITE setPublicKey NOTIFY publicKeyChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)

public:
    TelegramHost(QObject *parent = 0);
    ~TelegramHost();

    QString hostAddress() const;
    void setHostAddress(const QString &hostAddress);

    qint32 hostPort() const;
    void setHostPort(qint32 hostPort);

    qint32 hostDcId() const;
    void setHostDcId(qint32 hostDcId);

    QUrl publicKey() const;
    void setPublicKey(const QUrl &publicKey);

    virtual bool isValid() const;
    static QStringList requiredProperties();

Q_SIGNALS:
    void hostAddressChanged();
    void hostPortChanged();
    void hostDcIdChanged();
    void publicKeyChanged();
    void isValidChanged();

protected:
    void refreshValid();

private:
    TelegramHostPrivate *p;
};

#endif // TELEGRAMHOST_H
