/*
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: Vesa Halttunen <vesa.halttunen@jollamobile.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QStringList>
#include <QDateTime>
#include <QVariantHash>
#include <QDBusArgument>

struct NotificationData
{
    NotificationData();

    QString appName;
    quint32 replacesId;
    QString appIcon;
    QString summary;
    QString body;
    QHash<QString, QString> actions;
    QVariantHash hints;
    qint32 expireTimeout;
};

class NotificationManagerProxy;
class NotificationPrivate;

#ifdef Q_QDOC
// qdoc fails to link the documentation correctly because this project contains
// a C++ class named Notification and a QML class named Notification.  Work around
// this by making qdoc think the C++ class is in a namespace...
namespace libnemonotifications {
#endif

class Q_DECL_EXPORT Notification : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
    Q_PROPERTY(QString appName READ appName WRITE setAppName NOTIFY appNameChanged)
    Q_PROPERTY(quint32 replacesId READ replacesId WRITE setReplacesId NOTIFY replacesIdChanged)
    Q_PROPERTY(QString appIcon READ appIcon WRITE setAppIcon NOTIFY appIconChanged)
    Q_PROPERTY(QString summary READ summary WRITE setSummary NOTIFY summaryChanged)
    Q_PROPERTY(QString body READ body WRITE setBody NOTIFY bodyChanged)
    Q_PROPERTY(Urgency urgency READ urgency WRITE setUrgency NOTIFY urgencyChanged)
    Q_PROPERTY(qint32 expireTimeout READ expireTimeout WRITE setExpireTimeout NOTIFY expireTimeoutChanged)
    Q_PROPERTY(QDateTime timestamp READ timestamp WRITE setTimestamp NOTIFY timestampChanged)
    Q_PROPERTY(QString previewSummary READ previewSummary WRITE setPreviewSummary NOTIFY previewSummaryChanged)
    Q_PROPERTY(QString previewBody READ previewBody WRITE setPreviewBody NOTIFY previewBodyChanged)
    Q_PROPERTY(int itemCount READ itemCount WRITE setItemCount NOTIFY itemCountChanged)
    Q_PROPERTY(QString remoteDBusCallServiceName READ remoteDBusCallServiceName WRITE setRemoteDBusCallServiceName NOTIFY remoteDBusCallChanged)
    Q_PROPERTY(QString remoteDBusCallObjectPath READ remoteDBusCallObjectPath WRITE setRemoteDBusCallObjectPath NOTIFY remoteDBusCallChanged)
    Q_PROPERTY(QString remoteDBusCallInterface READ remoteDBusCallInterface WRITE setRemoteDBusCallInterface NOTIFY remoteDBusCallChanged)
    Q_PROPERTY(QString remoteDBusCallMethodName READ remoteDBusCallMethodName WRITE setRemoteDBusCallMethodName NOTIFY remoteDBusCallChanged)
    Q_PROPERTY(QVariantList remoteDBusCallArguments READ remoteDBusCallArguments WRITE setRemoteDBusCallArguments NOTIFY remoteDBusCallChanged)
    Q_PROPERTY(QVariantList remoteActions READ remoteActions WRITE setRemoteActions NOTIFY remoteActionsChanged)
    Q_PROPERTY(QString origin READ origin WRITE setOrigin NOTIFY originChanged)
    Q_PROPERTY(int maxContentLines READ maxContentLines WRITE setMaxContentLines NOTIFY maxContentLinesChanged)
    Q_ENUMS(Urgency)
    Q_ENUMS(CloseReason)

public:
    enum Urgency { Low = 0, Normal = 1, Critical = 2 };
    enum CloseReason { Expired = 0, DismissedByUser = 1, Closed = 2 };

    explicit Notification(QObject *parent = 0);
    virtual ~Notification();

    QString category() const;
    void setCategory(const QString &category);

    QString appName() const;
    void setAppName(const QString &appName);

    quint32 replacesId() const;
    void setReplacesId(quint32 id);

    QString appIcon() const;
    void setAppIcon(const QString &appIcon);

    QString summary() const;
    void setSummary(const QString &summary);

    QString body() const;
    void setBody(const QString &body);

    Urgency urgency() const;
    void setUrgency(Urgency urgency);

    qint32 expireTimeout() const;
    void setExpireTimeout(qint32 milliseconds);

    QDateTime timestamp() const;
    void setTimestamp(const QDateTime &timestamp);

    QString previewSummary() const;
    void setPreviewSummary(const QString &previewSummary);

    QString previewBody() const;
    void setPreviewBody(const QString &previewBody);

    int itemCount() const;
    void setItemCount(int itemCount);

    // Deprecated 'remoteDBusCall...' functions:
    QString remoteDBusCallServiceName() const;
    void setRemoteDBusCallServiceName(const QString &serviceName);

    QString remoteDBusCallObjectPath() const;
    void setRemoteDBusCallObjectPath(const QString &objectPath);

    QString remoteDBusCallInterface() const;
    void setRemoteDBusCallInterface(const QString &interface);

    QString remoteDBusCallMethodName() const;
    void setRemoteDBusCallMethodName(const QString &methodName);

    QVariantList remoteDBusCallArguments() const;
    void setRemoteDBusCallArguments(const QVariantList &arguments);

    // Obsoletes the set of remoteDBusCall... properties
    QVariantList remoteActions() const;
    void setRemoteActions(const QVariantList &remoteActions);
    inline void setRemoteAction(const QVariant &remoteAction) { setRemoteActions(QVariantList() << remoteAction); }

    QString origin() const;
    void setOrigin(const QString &origin);

    int maxContentLines() const;
    void setMaxContentLines(int max);

    QVariant hintValue(const QString &hint) const;
    void setHintValue(const QString &hint, const QVariant &value);

    Q_INVOKABLE void publish();
    Q_INVOKABLE void close();

    Q_INVOKABLE static QList<QObject*> notifications();
    Q_INVOKABLE static QList<QObject*> notifications(const QString &owner);

    Q_INVOKABLE static QVariant remoteAction(const QString &name, const QString &displayName,
                                             const QString &service, const QString &path, const QString &iface,
                                             const QString &method, const QVariantList &arguments = QVariantList());

signals:
    void clicked();
    void closed(uint reason);
    void categoryChanged();
    void appNameChanged();
    void replacesIdChanged();
    void appIconChanged();
    void summaryChanged();
    void bodyChanged();
    void urgencyChanged();
    void expireTimeoutChanged();
    void timestampChanged();
    void previewSummaryChanged();
    void previewBodyChanged();
    void itemCountChanged();
    void remoteActionsChanged();
    void remoteDBusCallChanged();
    void originChanged();
    void maxContentLinesChanged();

private slots:
    void checkActionInvoked(uint id, QString actionKey);
    void checkNotificationClosed(uint id, uint reason);

private:
    NotificationPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Notification)

    Notification(const NotificationData &data, QObject *parent = 0);

    static Notification *createNotification(const NotificationData &data, QObject *parent = 0);
};

#ifdef Q_QDOC
} // namespace libnemonotifications
#endif

QDBusArgument &operator<<(QDBusArgument &, const NotificationData &);
const QDBusArgument &operator>>(const QDBusArgument &, NotificationData &);

Q_DECLARE_METATYPE(NotificationData)
Q_DECLARE_METATYPE(QList<NotificationData>)

#endif // NOTIFICATION_H
