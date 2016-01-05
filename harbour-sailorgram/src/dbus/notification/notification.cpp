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
#include "notificationmanagerproxy.h"
#include "notification.h"

#include <QStringBuilder>

namespace {

const char *HINT_CATEGORY = "category";
const char *HINT_URGENCY = "urgency";
const char *HINT_ITEM_COUNT = "x-nemo-item-count";
const char *HINT_TIMESTAMP = "x-nemo-timestamp";
const char *HINT_PREVIEW_BODY = "x-nemo-preview-body";
const char *HINT_PREVIEW_SUMMARY = "x-nemo-preview-summary";
const char *HINT_REMOTE_ACTION_PREFIX = "x-nemo-remote-action-";
const char *HINT_REMOTE_ACTION_ICON_PREFIX = "x-nemo-remote-action-icon-";
const char *HINT_ORIGIN = "x-nemo-origin";
const char *HINT_OWNER = "x-nemo-owner";
const char *HINT_MAX_CONTENT_LINES = "x-nemo-max-content-lines";
const char *DEFAULT_ACTION_NAME = "default";

static inline QString processName() {
    // Defaults to the filename if not set
    return QCoreApplication::applicationName();
}

//! A proxy for accessing the notification manager
Q_GLOBAL_STATIC_WITH_ARGS(NotificationManagerProxy, notificationManagerProxyInstance, ("org.freedesktop.Notifications", "/org/freedesktop/Notifications", QDBusConnection::sessionBus()))

NotificationManagerProxy *notificationManager()
{
    if (!notificationManagerProxyInstance.exists()) {
        qDBusRegisterMetaType<NotificationData>();
        qDBusRegisterMetaType<QList<NotificationData> >();
    }

    return notificationManagerProxyInstance();
}

QString encodeDBusCall(const QString &service, const QString &path, const QString &iface, const QString &method, const QVariantList &arguments)
{
    const QString space(QStringLiteral(" "));

    QString s = service % space % path % space % iface % space % method;

    if (!arguments.isEmpty()) {
        QStringList args;
        int argsLength = 0;

        foreach (const QVariant &arg, arguments) {
            // Serialize the QVariant into a Base64 encoded byte stream
            QByteArray buffer;
            QDataStream stream(&buffer, QIODevice::WriteOnly);
            stream << arg;
            args.append(space + buffer.toBase64());
            argsLength += args.last().length();
        }

        s.reserve(s.length() + argsLength);
        foreach (const QString &arg, args) {
            s.append(arg);
        }
    }

    return s;
}

QStringList encodeActions(const QHash<QString, QString> &actions)
{
    QStringList rv;

    // Actions are encoded as a sequence of name followed by displayName
    QHash<QString, QString>::const_iterator it = actions.constBegin(), end = actions.constEnd();
    for ( ; it != end; ++it) {
        rv.append(it.key());
        rv.append(it.value());
    }

    return rv;
}

QHash<QString, QString> decodeActions(const QStringList &actions)
{
    QHash<QString, QString> rv;

    QStringList::const_iterator it = actions.constBegin(), end = actions.constEnd();
    while (it != end) {
        // If we have an odd number of tokens, add an empty displayName to complete the last pair
        const QString &name(*it);
        QString displayName;
        if (++it != end) {
            displayName = *it;
            ++it;
        }
        rv.insert(name, displayName);
    }

    return rv;
}

QPair<QHash<QString, QString>, QVariantHash> encodeActionHints(const QVariantList &actions)
{
    QPair<QHash<QString, QString>, QVariantHash> rv;

    foreach (const QVariant &action, actions) {
        QVariantMap vm = action.value<QVariantMap>();
        const QString actionName = vm["name"].value<QString>();
        if (!actionName.isEmpty()) {
            const QString displayName = vm["displayName"].value<QString>();
            const QString service = vm["service"].value<QString>();
            const QString path = vm["path"].value<QString>();
            const QString iface = vm["iface"].value<QString>();
            const QString method = vm["method"].value<QString>();
            const QVariantList arguments = vm["arguments"].value<QVariantList>();
            const QString icon = vm["icon"].value<QString>();

            if (!service.isEmpty() && !path.isEmpty() && !iface.isEmpty() && !method.isEmpty()) {
                rv.first.insert(actionName, displayName);
                rv.second.insert(QString(HINT_REMOTE_ACTION_PREFIX) + actionName, encodeDBusCall(service, path, iface, method, arguments));
                if (!icon.isEmpty()) {
                    rv.second.insert(QString(HINT_REMOTE_ACTION_ICON_PREFIX) + actionName, icon);
                }
            }
        }
    }

    return rv;
}

QVariantList decodeActionHints(const QHash<QString, QString> &actions, const QVariantHash &hints)
{
    QVariantList rv;

    QHash<QString, QString>::const_iterator ait = actions.constBegin(), aend = actions.constEnd();
    for ( ; ait != aend; ++ait) {
        const QString &actionName = ait.key();
        const QString &displayName = ait.value();

        const QString hintName = QString(HINT_REMOTE_ACTION_PREFIX) + actionName;
        const QString &hint = hints[hintName].toString();
        if (!hint.isEmpty()) {
            QVariantMap action;

            // Extract the element of the DBus call
            QStringList elements(hint.split(' ', QString::SkipEmptyParts));
            if (elements.size() <= 3) {
                qWarning() << "Unable to decode invalid remote action:" << hint;
            } else {
                int index = 0;
                action.insert(QStringLiteral("service"), elements.at(index++));
                action.insert(QStringLiteral("path"), elements.at(index++));
                action.insert(QStringLiteral("iface"), elements.at(index++));
                action.insert(QStringLiteral("method"), elements.at(index++));

                QVariantList args;
                while (index < elements.size()) {
                    const QString &arg(elements.at(index++));
                    const QByteArray buffer(QByteArray::fromBase64(arg.toUtf8()));

                    QDataStream stream(buffer);
                    QVariant var;
                    stream >> var;
                    args.append(var);
                }
                action.insert(QStringLiteral("arguments"), args);

                action.insert(QStringLiteral("name"), actionName);
                action.insert(QStringLiteral("displayName"), displayName);

                const QString iconHintName = QString(HINT_REMOTE_ACTION_ICON_PREFIX) + actionName;
                const QString &iconHint = hints[iconHintName].toString();
                if (!iconHint.isEmpty()) {
                    action.insert(QStringLiteral("icon"), iconHint);
                }

                rv.append(action);
            }
        }
    }

    return rv;
}

}

NotificationData::NotificationData()
    : replacesId(0)
    , expireTimeout(-1)
{
}

class NotificationPrivate : public NotificationData
{
    friend class Notification;

    NotificationPrivate()
        : NotificationData()
    {
    }

    NotificationPrivate(const NotificationData &data)
        : NotificationData(data)
        , remoteActions(decodeActionHints(actions, hints))
    {
    }

    QVariantMap firstRemoteAction() const
    {
        QVariantMap vm;
        const QVariant firstAction(remoteActions.value(0));
        if (!firstAction.isNull()) {
            vm = firstAction.value<QVariantMap>();
        }
        return vm;
    }

    void setFirstRemoteAction(QVariantMap vm, Notification *q)
    {
        QString name(vm["name"].value<QString>());
        if (name.isEmpty()) {
            vm.insert("name", QString::fromLatin1(DEFAULT_ACTION_NAME));
        }
        q->setRemoteActions(QVariantList() << vm);
    }

    QVariantList remoteActions;
};

/*!
    \qmltype Notification
    \inqmlmodule org.nemomobile.notifications
    \instantiates libnemonotifications::Notification
    \brief Allows notifications to be published

    The Notification type is a convenience type for using notifications
    based on the
    \l {https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html}
    {Desktop Notifications Specification} as implemented in Nemo.

    This type allows clients to create instances of notifications, which
    can be used to communicate to the Lipstick Notification Manager via
    D-Bus.  This simplifies the process of creating, listing and closing
    notifications, since the necessary communications are handled by the
    type.

    Notification content can be specified by setting the various properties
    on an instance of the Notification type, or can be handled by providing
    a category definition file, whose properties are automatically applied
    to matching notifications by the Lipstick Notification Manager. Properties
    set in the notification instance will not be overwritten by values
    listed in the category definition file. Please refer to Lipstick documentation
    for a complete description of the category definition files.

    A minimal example of using this type from a QML application:

    \qml
    Button {
        Notification {
            id: notification
            category: "x-nemo.example"
            summary: "Notification summary"
            body: "Notification body"
            onClicked: console.log("Clicked")
        }
        text: "Application notification" + (notification.replacesId ? " ID:" + notification.replacesId : "")
        onClicked: notification.publish()
    }
    \endqml

    When the button is clicked, a new notification is published to the
    notification manager, having the properties specified in the Notification
    object definition. Any properties specified by the definition file for
    the nominated category will be automatically applied by the notification
    manager during publication. The manager allocates an ID for the
    notification, and the instance is updated so that this ID is reflected
    in the \l replacesId property.

    When the user invokes the 'default' action on the notification, the
    \l clicked signal is emitted by the notification instance. If the
    application is no longer running at the relevant time, then the signal
    will be missed.

    An example category definition file
    /usr/share/lipstick/notificationcategories/x-nemo.example.conf:

    \code
    x-nemo-icon=icon-lock-sms
    x-nemo-preview-icon=icon-s-status-sms
    x-nemo-feedback=sms
    x-nemo-priority=70
    x-nemo-user-removable=true
    x-nemo-user-closeable=false
    \endcode

    A more exhaustive example of usage from a QML application:

    \qml
    Button {
        Notification {
            id: notification
            category: "x-nemo.example"
            appName: "Example App"
            appIcon: "/usr/share/example-app/icon-l-application"
            summary: "Notification summary"
            body: "Notification body"
            previewSummary: "Notification preview summary"
            previewBody: "Notification preview body"
            itemCount: 5
            timestamp: "2013-02-20 18:21:00"
            remoteActions: [ {
                "name": "default",
                "displayName": "Do something",
                "icon": "icon-s-do-it",
                "service": "org.nemomobile.example",
                "path": "/example",
                "iface": "org.nemomobile.example",
                "method": "doSomething",
                "arguments": [ "argument", 1 ]
            },{
                "name": "ignore",
                "displayName": "Ignore the problem",
                "icon": "icon-s-ignore",
                "service": "org.nemomobile.example",
                "path": "/example",
                "iface": "org.nemomobile.example",
                "method": "ignore",
                "arguments": [ "argument", 1 ]
            } ]
            onClicked: console.log("Clicked")
            onClosed: console.log("Closed, reason: " + reason)
        }
        text: "Application notification" + (notification.replacesId ? " ID:" + notification.replacesId : "")
        onClicked: notification.publish()
    }
    \endqml

    In this case, the notification includes a specification for
    'remote actions', which are D-Bus commands that the notification
    manager may permit the user to invoke. (Note that Lipstick currently
    supports invoking only the remote action named "default"). When
    an action is invoked on the notification, the corresponding D-Bus
    command is formulated and invoked, which allows the application
    to be launched to handled the notification action, if required.
 */

#ifdef Q_QDOC
namespace libnemonotifications {
#endif

/*!
    \namespace libnemonotifications
 */
/*!
    \class libnemonotifications::Notification
    \brief Allows notifications to be published

    The Notification class is a convenience class for using notifications
    based on the
    \l {https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html}
    {Desktop Notifications Specification} as implemented in Nemo.

    This class allows clients to create instances of notifications, which
    can be used to communicate to the Lipstick Notification Manager via
    D-Bus.  This simplifies the process of creating, listing and closing
    notifications, since the necessary communications are handled by the
    class.

    \note although \c Notification is shown here in the \c libnemonotification namespace, it is
    in fact defined in the global namespace.  The namespace here is used to work around limitations
    of the qdoc documentation generator.
 */

/*!
    \enum libnemonotifications::Notification::Urgency

    This enum type describes the urgency level of a notification.

    \value Low The notification is not urgent.
    \value Normal The notification is like most other notifications.
    \value Critical The notification is of urgent relevance to the user.

    \note although \c Notification is shown here in the \c libnemonotification namespace, it is
    in fact defined in the global namespace.  The namespace here is used to work around limitations
    of the qdoc documentation generator.
 */

/*!
    \enum libnemonotifications::Notification::CloseReason

    This enum type describes the reason given when a notification is reported as closed.

    \value Expired The notification expireTimeout period elapsed.
    \value DismissedByUser The notification was dismissed by user action.
    \value Closed The notification was closed programatically.

    \note although \c Notification is shown here in the \c libnemonotification namespace, it is
    in fact defined in the global namespace.  The namespace here is used to work around limitations
    of the qdoc documentation generator.
 */

/*!
    \fn libnemonotifications::Notification::Notification(QObject *)

    Constructs a new Notification, optionally using \a parent as the object parent.
 */
Notification::Notification(QObject *parent) :
    QObject(parent),
    d_ptr(new NotificationPrivate)
{
    connect(notificationManager(), SIGNAL(ActionInvoked(uint,QString)), this, SLOT(checkActionInvoked(uint,QString)));
    connect(notificationManager(), SIGNAL(NotificationClosed(uint,uint)), this, SLOT(checkNotificationClosed(uint,uint)));
}

/*!
    \fn libnemonotifications::Notification::Notification(const NotificationData &, QObject *)
    \internal
 */
Notification::Notification(const NotificationData &data, QObject *parent) :
    QObject(parent),
    d_ptr(new NotificationPrivate(data))
{
    connect(notificationManager(), SIGNAL(ActionInvoked(uint,QString)), this, SLOT(checkActionInvoked(uint,QString)));
    connect(notificationManager(), SIGNAL(NotificationClosed(uint,uint)), this, SLOT(checkNotificationClosed(uint,uint)));
}

/*!
    \fn libnemonotifications::Notification::~Notification()
    \internal
 */
Notification::~Notification()
{
    delete d_ptr;
}

/*!
    \qmlproperty string Notification::category

    The category whose properties should be applied to the notification by the Notification Manager.

    Properties defined by the category definition file will be applied to the notification,
    unless those properties are already set in the notification.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#hints}{hint value} "category".
 */
/*!
    \property libnemonotifications::Notification::category

    The category whose properties should be applied to the notification by the Notification Manager.

    Properties defined by the category definition file will be applied to the notification,
    unless those properties are already set in the notification.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#hints}{hint value} "category".
 */
QString Notification::category() const
{
    Q_D(const Notification);
    return d->hints.value(HINT_CATEGORY).toString();
}

void Notification::setCategory(const QString &category)
{
    Q_D(Notification);
    if (category != this->category()) {
        d->hints.insert(HINT_CATEGORY, category);
        emit categoryChanged();
    }
}

/*!
    \qmlproperty string Notification::appName

    The application name associated with this notification, for display purposes.

    The application name should be the formal name, localized if appropriate.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#commands}{Notify} parameter "app_name".
 */
/*!
    \property libnemonotifications::Notification::appName

    The application name associated with this notification, for display purposes.

    The application name should be the formal name, localized if appropriate.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#commands}{Notify} parameter "app_name".
 */
QString Notification::appName() const
{
    Q_D(const Notification);
    return d->appName;
}

void Notification::setAppName(const QString &appName)
{
    Q_D(Notification);
    if (appName != this->appName()) {
        d->appName = appName;
        emit appNameChanged();
    }
}

/*!
    \qmlproperty int Notification::replacesId

    The ID that should be used to replace or remove this notification.

    If a notification is published with a non-zero ID, it will replace any existing notification
    with that ID, without alerting the user to any changes. An unpublished notification has a ID
    of zero. The ID is automatically updated to contain the published ID after publication is
    reported by the Notification Manager.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#commands}{Notify} parameter "replaces_id".
 */
/*!
    \property libnemonotifications::Notification::replacesId

    The ID that should be used to replace or remove this notification.

    If a notification is published with a non-zero ID, it will replace any existing notification
    with that ID, without alerting the user to any changes. An unpublished notification has a ID
    of zero. The ID is automatically updated to contain the published ID after publication is
    reported by the Notification Manager.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#commands}{Notify} parameter "replaces_id".
 */
uint Notification::replacesId() const
{
    Q_D(const Notification);
    return d->replacesId;
}

void Notification::setReplacesId(uint id)
{
    Q_D(Notification);
    if (d->replacesId != id) {
        d->replacesId = id;
        emit replacesIdChanged();
    }
}

/*!
    \qmlproperty string Notification::appIcon

    The icon for the application that this notification is associated with. The value can
    be a URI, an absolute filesystem path, or a token to be interpreted by the theme image provider.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#commands}{Notify} parameter "app_icon".
 */
/*!
    \property libnemonotifications::Notification::appIcon

    The icon for the application that this notification is associated with. The value can
    be a URI, an absolute filesystem path, or a token to be interpreted by the theme image provider.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#commands}{Notify} parameter "app_icon".
 */
QString Notification::appIcon() const
{
    Q_D(const Notification);
    return d->appIcon;
}

void Notification::setAppIcon(const QString &appIcon)
{
    Q_D(Notification);
    if (appIcon != this->appIcon()) {
        d->appIcon = appIcon;
        emit appIconChanged();
    }
}

/*!
    \qmlproperty string Notification::summary

    The summary text briefly describing the notification.
    The summary should give a brief, single-line description of the notification.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#commands}{Notify} parameter "summary".
 */
/*!
    \property libnemonotifications::Notification::summary

    The summary text briefly describing the notification.
    The summary should give a brief, single-line description of the notification.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#commands}{Notify} parameter "summary".
 */
QString Notification::summary() const
{
    Q_D(const Notification);
    return d->summary;
}

void Notification::setSummary(const QString &summary)
{
    Q_D(Notification);
    if (d->summary != summary) {
        d->summary = summary;
        emit summaryChanged();
    }
}

/*!
    \qmlproperty string Notification::body

    Optional detailed body text.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#commands}{Notify} parameter "body".
 */
/*!
    \property libnemonotifications::Notification::body

    Optional detailed body text.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#commands}{Notify} parameter "body".
 */
QString Notification::body() const
{
    Q_D(const Notification);
    return d->body;
}

void Notification::setBody(const QString &body)
{
    Q_D(Notification);
    if (d->body != body) {
        d->body = body;
        emit bodyChanged();
    }
}

/*!
    \qmlproperty enumeration Notification::urgency

    The urgency level of the notification. The value corresponds to one of:

    \list
    \li Notification.Low
    \li Notification.Normal
    \li Notification.Critical
    \endlist

    Urgency level is interpreted by the Notification Manager at publication. It may decide
    to display or to suppress display of the notification depending on the current user
    activity or device state, where notifications with \c Critical urgency are more likely
    to be displayed.

    Defaults to Normal urgency.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#hints}{hint value} "urgency".
 */
/*!
    \property libnemonotifications::Notification::urgency

    The urgency level of the notification.

    Urgency level is interpreted by the Notification Manager at publication. It may decide
    to display or to suppress display of the notification depending on the current user
    activity or device state, where notifications with \c Critical urgency are more likely
    to be displayed.

    Defaults to Normal urgency.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#hints}{hint value} "urgency".
 */
Notification::Urgency Notification::urgency() const
{
    Q_D(const Notification);
    // Clipping to bounds in case an invalid value is stored as a hint
    return static_cast<Urgency>(qMax(static_cast<int>(Low), qMin(static_cast<int>(Critical), d->hints.value(HINT_URGENCY).toInt())));
}

void Notification::setUrgency(Urgency urgency)
{
    Q_D(Notification);
    if (urgency != this->urgency()) {
        d->hints.insert(HINT_URGENCY, static_cast<int>(urgency));
        emit urgencyChanged();
    }
}

/*!
    \qmlproperty int Notification::expireTimeout

    The number of milliseconds after display at which the notification should be automatically closed.
    A value of zero indicates that the notification should not close automatically, while -1
    indicates that the notification manager should decide the expiration timeout.

    Defaults to -1.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#commands}{Notify} parameter "expire_timeout".
 */
/*!
    \property libnemonotifications::Notification::expireTimeout

    The number of milliseconds after display at which the notification should be automatically closed.
    A value of zero indicates that the notification should not close automatically, while -1
    indicates that the notification manager should decide the expiration timeout.

    Defaults to -1.

    This property is transmitted as the standard \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#commands}{Notify} parameter "expire_timeout".
 */
qint32 Notification::expireTimeout() const
{
    Q_D(const Notification);
    return d->expireTimeout;
}

void Notification::setExpireTimeout(qint32 milliseconds)
{
    Q_D(Notification);
    if (milliseconds != d->expireTimeout) {
        d->expireTimeout = milliseconds;
        emit expireTimeoutChanged();
    }
}

/*!
    \qmlproperty date Notification::timestamp

    The timestamp is typically associated with an event that the notification relates
    to, rather than for the creation of the notification itself. If not specified, the
    notification's timestamp will become the time of publification.

    This property is transmitted as the extension hint value "x-nemo-timestamp".
 */
/*!
    \property libnemonotifications::Notification::timestamp

    The timestamp is typically associated with an event that the notification relates
    to, rather than for the creation of the notification itself. If not specified, the
    notification's timestamp will become the time of publification.

    This property is transmitted as the extension hint value "x-nemo-timestamp".
 */
QDateTime Notification::timestamp() const
{
    Q_D(const Notification);
    return d->hints.value(HINT_TIMESTAMP).toDateTime();
}

void Notification::setTimestamp(const QDateTime &timestamp)
{
    Q_D(Notification);
    if (timestamp != this->timestamp()) {
        d->hints.insert(HINT_TIMESTAMP, timestamp.toString(Qt::ISODate));
        emit timestampChanged();
    }
}

/*!
    \qmlproperty string Notification::previewSummary

    Summary text to be shown in the preview banner for the notification, if any.

    If \c previewSummary or \l previewBody are specified, a preview of the notification
    will be generated by Lipstick at publication (unless the Notification Manager chooses
    to suppress the preview).

    This property is transmitted as the extension hint value "x-nemo-preview-summary".
 */
/*!
    \property libnemonotifications::Notification::previewSummary

    Summary text to be shown in the preview banner for the notification, if any.

    If \c previewSummary or \l previewBody are specified, a preview of the notification
    will be generated by Lipstick at publication (unless the Notification Manager chooses
    to suppress the preview).

    This property is transmitted as the extension hint value "x-nemo-preview-summary".
 */
QString Notification::previewSummary() const
{
    Q_D(const Notification);
    return d->hints.value(HINT_PREVIEW_SUMMARY).toString();
}

void Notification::setPreviewSummary(const QString &previewSummary)
{
    Q_D(Notification);
    if (previewSummary != this->previewSummary()) {
        d->hints.insert(HINT_PREVIEW_SUMMARY, previewSummary);
        emit previewSummaryChanged();
    }
}

/*!
    \qmlproperty string Notification::previewBody

    Body text to be shown in the preview banner for the notification, if any.

    If \l previewSummary or \c previewBody are specified, a preview of the notification
    will be generated by Lipstick at publication (unless the Notification Manager chooses
    to suppress the preview).

    This property is transmitted as the extension hint value "x-nemo-preview-body".
 */
/*!
    \property libnemonotifications::Notification::previewBody

    Body text to be shown in the preview banner for the notification, if any.

    If \l previewSummary or \c previewBody are specified, a preview of the notification
    will be generated by Lipstick at publication (unless the Notification Manager chooses
    to suppress the preview).

    This property is transmitted as the extension hint value "x-nemo-preview-body".
 */
QString Notification::previewBody() const
{
    Q_D(const Notification);
    return d->hints.value(HINT_PREVIEW_BODY).toString();
}

void Notification::setPreviewBody(const QString &previewBody)
{
    Q_D(Notification);
    if (previewBody != this->previewBody()) {
        d->hints.insert(HINT_PREVIEW_BODY, previewBody);
        emit previewBodyChanged();
    }
}

/*!
    \qmlproperty int Notification::itemCount

    The number of items represented by the notification.
    For example, a single notification can represent four missed calls by setting the count to 4. Defaults to 1.

    This property is transmitted as the extension hint value "x-nemo-item-count".
 */
/*!
    \property libnemonotifications::Notification::itemCount

    The number of items represented by the notification.
    For example, a single notification can represent four missed calls by setting the count to 4. Defaults to 1.

    This property is transmitted as the extension hint value "x-nemo-item-count".
 */
int Notification::itemCount() const
{
    Q_D(const Notification);
    return d->hints.value(HINT_ITEM_COUNT).toInt();
}

void Notification::setItemCount(int itemCount)
{
    Q_D(Notification);
    if (itemCount != this->itemCount()) {
        d->hints.insert(HINT_ITEM_COUNT, itemCount);
        emit itemCountChanged();
    }
}

/*!
    \qmlmethod void Notification::publish()

    Publishes the current state of the notification to the Notification Manager.

    If \l replacesId is zero, a new notification will be created and \l replacesId will be updated
    to contain that ID. Otherwise the existing notification with the given ID is updated with the
    new details.
*/
/*!
    \fn libnemonotifications::Notification::publish()

    Publishes the current state of the notification to the Notification Manager.

    If \l replacesId is zero, a new notification will be created and \l replacesId will be updated
    to contain that ID. Otherwise the existing notification with the given ID is updated with the
    new details.
 */
void Notification::publish()
{
    Q_D(Notification);

    // Validate the actions associated with the notification
    Q_FOREACH (const QVariant &action, d->remoteActions) {
        const QVariantMap &vm = action.value<QVariantMap>();
        if (vm["name"].value<QString>().isEmpty()
                || vm["service"].value<QString>().isEmpty()
                || vm["path"].value<QString>().isEmpty()
                || vm["iface"].value<QString>().isEmpty()
                || vm["method"].value<QString>().isEmpty()) {
            qWarning() << "Invalid remote action specification:" << action;
        }
    }

    // Ensure the ownership of this notification is recorded
    QVariantHash::iterator it = d->hints.find(HINT_OWNER);
    if (it == d->hints.end()) {
        d->hints.insert(HINT_OWNER, processName());
    }

    setReplacesId(notificationManager()->Notify(appName(), d->replacesId, d->appIcon, d->summary, d->body,
                                                encodeActions(d->actions), d->hints, d->expireTimeout));
}

/*!
    \qmlmethod void Notification::close()

    Closes the notification identified by \l replacesId.
*/
/*!
    \fn libnemonotifications::Notification::close()

    Closes the notification identified by \l replacesId.
 */
void Notification::close()
{
    Q_D(Notification);
    if (d->replacesId != 0) {
        notificationManager()->CloseNotification(d->replacesId);
        setReplacesId(0);
    }
}

/*!
    \qmlsignal Notification::clicked()

    Emitted when the notification is activated by the user.

    Handling the \c clicked signal is only effective if the process is running when the
    user activates the notification, which may occur long after the notification is
    published. A more robust solution is to register a 'remote action' with the
    Notification Manager, so that a handler can be started running and invoked
    to service the request.

    \sa Notification::remoteActions
*/
/*!
    \fn void libnemonotifications::Notification::clicked()

    Emitted when the notification is activated by the user.

    Handling the \c clicked signal is only effective if the process is running when the
    user activates the notification, which may occur long after the notification is
    published. A more robust solution is to register a 'remote action' with the
    Notification Manager, so that a handler can be started running and invoked
    to service the request.

    \sa libnemonotifications::Notification::remoteActions()
 */
void Notification::checkActionInvoked(uint id, QString actionKey)
{
    Q_D(Notification);
    if (id == d->replacesId) {
        if (actionKey == DEFAULT_ACTION_NAME) {
            emit clicked();
        }
    }
}

/*!
    \qmlsignal Notification::closed(uint reason)

    Emitted when the notification is reported closed by the notification manager.
    The \a reason value may be any one of:

    \list
    \li Notification.Expired
    \li Notification.DismissedByUser
    \li Notification.Closed
    \endlist
*/
/*!
    \fn void libnemonotifications::Notification::closed(uint reason)

    Emitted when the notification is reported closed by the notification manager.
    The \a reason value corresponds to a value defined by \l Notification::CloseReason.
 */
void Notification::checkNotificationClosed(uint id, uint reason)
{
    Q_D(Notification);
    if (id == d->replacesId) {
        emit closed(reason);
        setReplacesId(0);
    }
}

/*!
    \property libnemonotifications::Notification::remoteDBusCallServiceName
    \internal
 */
/*!
    \fn libnemonotifications::Notification::remoteDBusCallServiceName() const
    \internal
 */
QString Notification::remoteDBusCallServiceName() const
{
    Q_D(const Notification);
    QVariantMap vm(d->firstRemoteAction());
    return vm["service"].value<QString>();
}

/*!
    \fn libnemonotifications::Notification::setRemoteDBusCallServiceName(const QString &)
    \internal
 */
void Notification::setRemoteDBusCallServiceName(const QString &serviceName)
{
    Q_D(Notification);
    QVariantMap vm(d->firstRemoteAction());
    if (vm["service"].value<QString>() != serviceName) {
        vm.insert("service", serviceName);
        d->setFirstRemoteAction(vm, this);

        emit remoteActionsChanged();
        emit remoteDBusCallChanged();
    }
}

/*!
    \property libnemonotifications::Notification::remoteDBusCallObjectPath
    \internal
 */
/*!
    \fn libnemonotifications::Notification::remoteDBusCallObjectPath() const
    \internal
 */
QString Notification::remoteDBusCallObjectPath() const
{
    Q_D(const Notification);
    QVariantMap vm(d->firstRemoteAction());
    return vm["path"].value<QString>();
}

/*!
    \fn libnemonotifications::Notification::setRemoteDBusCallObjectPath(const QString &)
    \internal
 */
void Notification::setRemoteDBusCallObjectPath(const QString &objectPath)
{
    Q_D(Notification);
    QVariantMap vm(d->firstRemoteAction());
    if (vm["path"].value<QString>() != objectPath) {
        vm.insert("path", objectPath);
        d->setFirstRemoteAction(vm, this);

        emit remoteActionsChanged();
        emit remoteDBusCallChanged();
    }
}

/*!
    \property libnemonotifications::Notification::remoteDBusCallInterface
    \internal
 */
/*!
    \fn libnemonotifications::Notification::remoteDBusCallInterface() const
    \internal
 */
QString Notification::remoteDBusCallInterface() const
{
    Q_D(const Notification);
    QVariantMap vm(d->firstRemoteAction());
    return vm["iface"].value<QString>();
}

/*!
    \fn libnemonotifications::Notification::setRemoteDBusCallInterface(const QString &)
    \internal
 */
void Notification::setRemoteDBusCallInterface(const QString &interface)
{
    Q_D(Notification);
    QVariantMap vm(d->firstRemoteAction());
    if (vm["iface"].value<QString>() != interface) {
        vm.insert("iface", interface);
        d->setFirstRemoteAction(vm, this);

        emit remoteActionsChanged();
        emit remoteDBusCallChanged();
    }
}

/*!
    \property libnemonotifications::Notification::remoteDBusCallMethodName
    \internal
 */
/*!
    \fn libnemonotifications::Notification::remoteDBusCallMethodName() const
    \internal
 */
QString Notification::remoteDBusCallMethodName() const
{
    Q_D(const Notification);
    QVariantMap vm(d->firstRemoteAction());
    return vm["method"].value<QString>();
}

/*!
    \fn libnemonotifications::Notification::setRemoteDBusCallMethodName(const QString &)
    \internal
 */
void Notification::setRemoteDBusCallMethodName(const QString &methodName)
{
    Q_D(Notification);
    QVariantMap vm(d->firstRemoteAction());
    if (vm["method"].value<QString>() != methodName) {
        vm.insert("method", methodName);
        d->setFirstRemoteAction(vm, this);

        emit remoteActionsChanged();
        emit remoteDBusCallChanged();
    }
}

/*!
    \property libnemonotifications::Notification::remoteDBusCallArguments
    \internal
 */
/*!
    \fn libnemonotifications::Notification::remoteDBusCallArguments() const
    \internal
 */
QVariantList Notification::remoteDBusCallArguments() const
{
    Q_D(const Notification);
    QVariantMap vm(d->firstRemoteAction());
    return vm["arguments"].value<QVariantList>();
}

/*!
    \fn libnemonotifications::Notification::setRemoteDBusCallArguments(const QVariantList &)
    \internal
 */
void Notification::setRemoteDBusCallArguments(const QVariantList &arguments)
{
    Q_D(Notification);
    QVariantMap vm(d->firstRemoteAction());
    if (vm["arguments"].value<QVariantList>() != arguments) {
        vm.insert("arguments", arguments);
        d->setFirstRemoteAction(vm, this);

        emit remoteActionsChanged();
        emit remoteDBusCallChanged();
    }
}

/*!
    \fn libnemonotifications::Notification::remoteDBusCallChanged()
    \internal
 */

/*!
    \qmlproperty list<variant> Notification::remoteActions

    The remote actions registered for potential invocation by this notification.

    Remote actions are specified as a list of objects having the properties
    'name', 'service', 'path', 'iface' and 'method', and optionally, the
    properties 'displayName', 'icon' and 'arguments'.

    For example:

    \qml
    Notification {
        remoteActions: [ {
            "name": "default",
            "displayName": "Do something",
            "icon": "icon-s-do-it",
            "service": "org.nemomobile.example",
            "path": "/example",
            "iface": "org.nemomobile.example",
            "method": "doSomething",
            "arguments": [ "argument", 1 ]
        } ]
    }
    \endqml

    \note the current implementation of Lipstick will invoke the action named "default" when
    the user activates an individual notification.  If the user activates a notification
    group, the action named "app" will be invoked, if that action is shared by all members of
    the group.

    This property is transmitted as the \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#commands}{Notify} parameter "actions" and the extension hint value "x-nemo-remote-action-<name>".
 */
/*!
    \property libnemonotifications::Notification::remoteActions

    The remote actions registered for potential invocation by this notification.

    Remote actions specify D-Bus calls to be emitted by the Notification Manager when a notification
    is activated by the user.  \l{libnemonotifications::Notification::remoteAction()}
    {remoteAction} describes the format of a remote action specification.

    \note the current implementation of Lipstick will invoke the action named "default" when
    the user activates an individual notification.  If the user activates a notification
    group, the action named "app" will be invoked, if that action is shared by all members of
    the group.

    This property is transmitted as the \l{https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html#commands}{Notify} parameter "actions" and the extension hint value "x-nemo-remote-action-<name>".

    \sa libnemonotifications::Notification::remoteAction()
 */
QVariantList Notification::remoteActions() const
{
    Q_D(const Notification);
    return d->remoteActions;
}

void Notification::setRemoteActions(const QVariantList &remoteActions)
{
    Q_D(Notification);
    if (remoteActions != d->remoteActions) {
        // Remove any existing actions
        foreach (const QVariant &action, d->remoteActions) {
            QVariantMap vm = action.value<QVariantMap>();
            const QString actionName = vm["name"].value<QString>();
            if (!actionName.isEmpty()) {
                d->hints.remove(QString(HINT_REMOTE_ACTION_PREFIX) + actionName);
                d->actions.remove(actionName);
            }
        }

        // Add the new actions and their associated hints
        d->remoteActions = remoteActions;

        QPair<QHash<QString, QString>, QVariantHash> actionHints = encodeActionHints(remoteActions);

        QHash<QString, QString>::const_iterator ait = actionHints.first.constBegin(), aend = actionHints.first.constEnd();
        for ( ; ait != aend; ++ait) {
            d->actions.insert(ait.key(), ait.value());
        }

        QVariantHash::const_iterator hit = actionHints.second.constBegin(), hend = actionHints.second.constEnd();
        for ( ; hit != hend; ++hit) {
            d->hints.insert(hit.key(), hit.value());
        }

        emit remoteActionsChanged();
        emit remoteDBusCallChanged();
    }
}

/*!
    \fn libnemonotifications::Notification::setRemoteAction(const QVariant &)
    \internal
 */

/*!
    \qmlproperty string Notification::origin

    A property indicating the origin of the notification.

    The origin hint can be used to associate an external property with a notification, separate
    from the intermediary that reports the notificaiton. For example, a notification of a new
    email is created and handled by an email client application, but notionally originates at
    the sender's email address.

    This property is transmitted as the extension hint value "x-nemo-origin".
*/
/*!
    \property libnemonotifications::Notification::origin

    A property indicating the origin of the notification.

    The origin hint can be used to associate an external property with a notification, separate
    from the intermediary that reports the notificaiton. For example, a notification of a new
    email is created and handled by an email client application, but notionally originates at
    the sender's email address.

    This property is transmitted as the extension hint value "x-nemo-origin".
*/
QString Notification::origin() const
{
    Q_D(const Notification);
    return d->hints.value(HINT_ORIGIN).toString();
}

void Notification::setOrigin(const QString &origin)
{
    Q_D(Notification);
    if (origin != this->origin()) {
        d->hints.insert(HINT_ORIGIN, origin);
        emit originChanged();
    }
}

/*!
    \qmlproperty int Notification::maxContentLines

    A property suggesting the maximum amount of content to display for the notification.
    The content lines include the summary line, so a single-line notification does
    not display any body text.

    This property is transmitted as the extension hint value "x-nemo-max-content-lines".
*/
/*!
    \property libnemonotifications::Notification::maxContentLines

    A property suggesting the maximum amount of content to display for the notification.
    The content lines include the summary line, so a single-line notification does
    not display any body text.

    This property is transmitted as the extension hint value "x-nemo-max-content-lines".
*/
int Notification::maxContentLines() const
{
    Q_D(const Notification);
    return d->hints.value(HINT_MAX_CONTENT_LINES).toInt();
}

void Notification::setMaxContentLines(int max)
{
    Q_D(Notification);
    if (max != this->maxContentLines()) {
        d->hints.insert(HINT_MAX_CONTENT_LINES, max);
        emit maxContentLinesChanged();
    }
}

/*!
    \fn libnemonotifications::Notification::hintValue(const QString &) const

    Returns the value of the hint named \a hint.
 */
QVariant Notification::hintValue(const QString &hint) const
{
    Q_D(const Notification);
    return d->hints.value(hint);
}

/*!
    \fn libnemonotifications::Notification::setHintValue(const QString &, const QVariant &)

    Sets the value of the hint named \a hint to \a value.
 */
void Notification::setHintValue(const QString &hint, const QVariant &value)
{
    Q_D(Notification);
    d->hints.insert(hint, value);
}

/*!
    \fn libnemonotifications::Notification::notifications()

    Returns a list of existing notifications whose 'x-nemo-owner' hint value
    matches the process name of the running process.

    All notifications produced by calling \l publish() are set to contain a hint
    'x-nemo-owner' with the value of process name of the running process (unless
    that hint is already specified).  This allows previously generated notifications
    to be easily retrieved via this function.

    The returned objects are instances of the \c Notification class. The caller takes ownership and
    should destroy them when they are no longer required.
 */
QList<QObject*> Notification::notifications()
{
    // By default, only the notifications owned by us are returned
    return notifications(processName());
}

/*!
    \fn libnemonotifications::Notification::notifications(const QString &)

    Returns a list of existing notifications whose 'x-nemo-owner' hint value
    matches \a owner.

    The returned objects are instances of the \c Notification class. The caller takes ownership and
    should destroy them when they are no longer required.
 */
QList<QObject*> Notification::notifications(const QString &owner)
{
    QList<NotificationData> notifications = notificationManager()->GetNotifications(owner);
    QList<QObject*> objects;
    foreach (const NotificationData &notification, notifications) {
        objects.append(createNotification(notification, notificationManager()));
    }
    return objects;
}

/*!
    \fn libnemonotifications::Notification::remoteAction(const QString &, const QString &, const QString &, const QString &, const QString &, const QString &, const QVariantList &)

    Helper function to assemble an object specifying a remote action, to be invoked via D-Bus.

    A valid remote action must contain \a name, \a service, \a path, \a iface
    and \a method values. \a displayName and \a arguments are optional properties.
    The values are used to construct a D-Bus call to be invoked by the Notification
    Manager when the notification is activated by the user:

    \list
    \li \a name: the name of the action (currently Lipstick will only invoke the actions named "default" and "app")
    \li \a displayName: the name of the action to be displayed to user (not currently used by Lipstick)
    \li \a service: the name of the D-Bus service to be invoked
    \li \a path: the object path to be invoked via D-Bus
    \li \a iface: the interface to be invoked via D-Bus
    \li \a method: the method of the interface to be invoked via D-Bus
    \li \a arguments: the optional arguments to be passed to the method invoked via D-Bus
    \endlist
*/
QVariant Notification::remoteAction(const QString &name, const QString &displayName,
                                    const QString &service, const QString &path, const QString &iface,
                                    const QString &method, const QVariantList &arguments)
{
    QVariantMap action;

    if (!name.isEmpty()) {
        action.insert(QStringLiteral("name"), name);
    }
    if (!displayName.isEmpty()) {
        action.insert(QStringLiteral("displayName"), displayName);
    }
    if (!service.isEmpty()) {
        action.insert(QStringLiteral("service"), service);
    }
    if (!path.isEmpty()) {
        action.insert(QStringLiteral("path"), path);
    }
    if (!iface.isEmpty()) {
        action.insert(QStringLiteral("iface"), iface);
    }
    if (!method.isEmpty()) {
        action.insert(QStringLiteral("method"), method);
    }
    if (!arguments.isEmpty()) {
        action.insert(QStringLiteral("arguments"), arguments);
    }

    return action;
}

/*!
    \fn libnemonotifications::Notification::createNotification(const NotificationData &, QObject *)
    \internal
 */
Notification *Notification::createNotification(const NotificationData &data, QObject *parent)
{
    return new Notification(data, parent);
}

QDBusArgument &operator<<(QDBusArgument &argument, const NotificationData &data)
{
    argument.beginStructure();
    argument << data.appName;
    argument << data.replacesId;
    argument << data.appIcon;
    argument << data.summary;
    argument << data.body;
    argument << encodeActions(data.actions);
    argument << data.hints;
    argument << data.expireTimeout;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NotificationData &data)
{
    QStringList tempStringList;

    argument.beginStructure();
    argument >> data.appName;
    argument >> data.replacesId;
    argument >> data.appIcon;
    argument >> data.summary;
    argument >> data.body;
    argument >> tempStringList;
    argument >> data.hints;
    argument >> data.expireTimeout;
    argument.endStructure();

    data.actions = decodeActions(tempStringList);

    return argument;
}

#ifdef Q_QDOC
} // namespace libnemonotifications
#endif
