#ifndef TELEGRAMTESTTOOLS_H
#define TELEGRAMTESTTOOLS_H

#include <QJSValue>
#include <QObject>

class TelegramTestToolsPrivate;
class TelegramTestTools : public QObject
{
    Q_OBJECT
public:
    TelegramTestTools(QObject *parent = 0);
    ~TelegramTestTools();

public Q_SLOTS:
    void connectDestroy(QObject *obj, const QJSValue &callback);

Q_SIGNALS:

private:
    TelegramTestToolsPrivate *p;
};

#endif // TELEGRAMTESTTOOLS_H
