#include "dialogshistorymodel.h"

DialogsHistoryModel::DialogsHistoryModel(QObject *parent) : QAbstractListModel(parent), _telegram(NULL)
{

}

QHash<int, QByteArray> DialogsHistoryModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[DialogsHistoryModel::DialogRole] = "dialog";
    return roles;
}

QVariant DialogsHistoryModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || (index.row() >= this->_recentdialogs.length()))
        return QVariant();

    if(role == DialogsHistoryModel::DialogRole)
        return QVariant::fromValue(this->_recentdialogs.at(index.row()));

    return QVariant();
}

int DialogsHistoryModel::rowCount(const QModelIndex &) const
{
    return this->_recentdialogs.length();
}

void DialogsHistoryModel::markAsRead(DialogObject *dialog)
{
    this->beginResetModel();
    this->removeDialog(dialog);
    this->endResetModel();
}

TelegramQml *DialogsHistoryModel::telegram() const
{
    return this->_telegram;
}

void DialogsHistoryModel::setTelegram(TelegramQml *telegram)
{
    if(this->_telegram == telegram)
        return;

    if(this->_telegram)
        disconnect(this->_telegram, SIGNAL(incomingMessage(MessageObject*)), this, 0);

    this->_telegram = telegram;
    connect(this->_telegram, SIGNAL(incomingMessage(MessageObject*)), this, SLOT(onIncomingMessage(MessageObject*)));
}

void DialogsHistoryModel::removeOlder()
{
    while(this->_recentdialogs.length() >= MAX_RECENT_CHATS)
    {
        DialogObject* olddialog = this->_recentdialogs.last();
        this->_recentdialogs.removeLast();
        this->_recentdialogsset.remove(olddialog);
    }
}

void DialogsHistoryModel::removeDialog(DialogObject* dialog)
{
    for(int i = 0; i < this->_recentdialogs.length(); i++)
    {
        DialogObject* dlg = this->_recentdialogs.at(i);

        if(dlg == dialog)
        {
            this->_recentdialogs.removeAt(i);
            this->_recentdialogsset.remove(dlg);
            return;
        }
    }
}

void DialogsHistoryModel::onIncomingMessage(MessageObject *message)
{
    this->beginResetModel();
    this->removeOlder();

    DialogObject* dialog = this->_telegram->messageDialog(message->id());

    if(this->_recentdialogsset.contains(dialog))
        this->removeDialog(dialog);

    this->_recentdialogs.prepend(dialog);
    this->_recentdialogsset.insert(dialog);
    this->endResetModel();
}

