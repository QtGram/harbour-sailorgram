#ifndef NEWSLETTERDIALOG_H
#define NEWSLETTERDIALOG_H

#include "telegramqml_global.h"
#include <telegram/types/types.h>

#include <QObject>
#include <QMap>

class Dialog;
class User;
class Message;
class TELEGRAMQMLSHARED_EXPORT NewsLetterDialog : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    virtual Dialog dialog() const = 0;
    virtual User user() const = 0;

    virtual void check() = 0;
    static qint32 cutegramId();

Q_SIGNALS:
    void incomingMessage(const Message &msg, const Dialog &dialog);
};

#endif // NEWSLETTERDIALOG_H
