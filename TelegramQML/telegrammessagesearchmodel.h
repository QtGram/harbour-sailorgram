#ifndef TELEGRAMMESSAGESEARCHMODEL_H
#define TELEGRAMMESSAGESEARCHMODEL_H

#include "telegrammessagelistmodel.h"

#include <QDateTime>

class TelegramMessageSearchModelPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramMessageSearchModel : public TelegramMessageListModel
{
    Q_OBJECT
    Q_PROPERTY(QString keyword READ keyword WRITE setKeyword NOTIFY keywordChanged)
    Q_PROPERTY(InputPeerObject* currentPeer READ currentPeer WRITE setCurrentPeer NOTIFY currentPeerChanged)
    Q_PROPERTY(int messageFilter READ messageFilter WRITE setMessageFilter NOTIFY messageFilterChanged)
    Q_PROPERTY(QDateTime minimumDate READ minimumDate WRITE setMinimumDate NOTIFY minimumDateChanged)
    Q_PROPERTY(QDateTime maximumDate READ maximumDate WRITE setMaximumDate NOTIFY maximumDateChanged)

public:
    TelegramMessageSearchModel(QObject *parent = 0);
    ~TelegramMessageSearchModel();

    void setCurrentPeer(InputPeerObject *peer);
    InputPeerObject *currentPeer() const;

    void setKeyword(const QString &keyword);
    QString keyword() const;

    /*! From MessagesFilterObject class */
    void setMessageFilter(int filter);
    int messageFilter() const;

    void setMinimumDate(const QDateTime &minDate);
    QDateTime minimumDate() const;

    void setMaximumDate(const QDateTime &maxDate);
    QDateTime maximumDate() const;

    static QStringList requiredProperties();

Q_SIGNALS:
    void keywordChanged();
    void messageFilterChanged();
    void minimumDateChanged();
    void maximumDateChanged();
    void currentPeerChanged();

public Q_SLOTS:
    void loadBack();

protected:
    virtual void refresh();
    void getFromServer(bool more = false);

private:
    TelegramMessageSearchModelPrivate *p;
};

#endif // TELEGRAMMESSAGESEARCHMODEL_H
