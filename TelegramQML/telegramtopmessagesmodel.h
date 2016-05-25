#ifndef TELEGRAMTOPMESSAGESMODEL_H
#define TELEGRAMTOPMESSAGESMODEL_H

#include "telegrammessagelistmodel.h"

class TelegramTopMessagesModelPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramTopMessagesModel : public TelegramMessageListModel
{
    Q_OBJECT
    Q_ENUMS(VisibleTypes)
    Q_PROPERTY(int dialogsLimit READ dialogsLimit WRITE setDialogsLimit NOTIFY dialogsLimitChanged)
    Q_PROPERTY(int visibleType READ visibleType WRITE setVisibleType NOTIFY visibleTypeChanged)

public:
    enum VisibleTypes {
        VisibleTypeNone = 0,
        VisibleTypeChannel = 1,
        VisibleTypeChat = 2,
        VisibleTypeUser = 4,
        VisibleTypeMegaGroup = 8,
        VisibleTypeAll = VisibleTypeChannel | VisibleTypeChat | VisibleTypeUser | VisibleTypeMegaGroup
    };

    TelegramTopMessagesModel(QObject *parent = 0);
    ~TelegramTopMessagesModel();

    void setDialogsLimit(int dialogsLimit);
    int dialogsLimit() const;

    void setVisibleType(int visibleType);
    int visibleType() const;

Q_SIGNALS:
    void dialogsLimitChanged();
    void visibleTypeChanged();

public Q_SLOTS:
    void recheck();

protected:
    virtual void refresh();
    void loadDialogs(const QList<InputPeer> &peers, const QList<Dialog> &dialogs);

private:
    void loadDialogs_prv(QList<InputPeer> peers, QList<Dialog> dialogs);

private:
    TelegramTopMessagesModelPrivate *p;
};

#endif // TELEGRAMTOPMESSAGESMODEL_H
