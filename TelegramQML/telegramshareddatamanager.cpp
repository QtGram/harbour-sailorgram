#define DECLARE_INSERT_FNC(TYPE, VARIABLE) \
    const QByteArray &_key = TelegramTools::identifier(VARIABLE); \
    if(key) *key = _key; \
    TYPE##Object *result = p->VARIABLE##s.value(_key); \
    if(result) { \
        *result = VARIABLE; \
        return result; \
    } \
    result = new TYPE##Object(VARIABLE); \
    connect(result, &TYPE##Object::destroyed, this, [this, _key](){ \
        p->VARIABLE##s.remove(_key); \
    }); \
    p->VARIABLE##s[_key] = result; \
    return result;

#define DECLARE_GET_FNC(VARIABLE) \
    return p->VARIABLE##s.value(byte);

#define DECLARE_GET_ALL_FNC(VARIABLE) \
    return p->VARIABLE##s.values();

#include "telegramshareddatamanager.h"
#include "telegramtools.h"

#include <QPointer>
#include <QByteArray>
#include <QHash>

class TelegramSharedDataManagerPrivate
{
public:
    QHash<QByteArray, DialogObject*> dialogs;
    QHash<QByteArray, MessageObject*> messages;
    QHash<QByteArray, ChatObject*> chats;
    QHash<QByteArray, UserObject*> users;
    QHash<QByteArray, UserFullObject*> userFulls;
    QHash<QByteArray, ChatFullObject*> chatFulls;
    QHash<QByteArray, InputPeerObject*> peers;
    QHash<QByteArray, StickerSetObject*> stickerSets;
    QHash<QByteArray, DocumentObject*> documents;
};

TelegramSharedDataManager::TelegramSharedDataManager(QObject *parent) :
    QObject(parent)
{
    p = new TelegramSharedDataManagerPrivate;
}

TelegramSharedPointer<DialogObject> TelegramSharedDataManager::insertDialog(const Dialog &dialog, QByteArray *key)
{
    DECLARE_INSERT_FNC(Dialog, dialog)
}

TelegramSharedPointer<MessageObject> TelegramSharedDataManager::insertMessage(const Message &message, QByteArray *key)
{
    DECLARE_INSERT_FNC(Message, message)
}

TelegramSharedPointer<ChatObject> TelegramSharedDataManager::insertChat(const Chat &chat, QByteArray *key)
{
    DECLARE_INSERT_FNC(Chat, chat)
}

TelegramSharedPointer<UserObject> TelegramSharedDataManager::insertUser(const User &user, QByteArray *key)
{
    DECLARE_INSERT_FNC(User, user)
}

TelegramSharedPointer<InputPeerObject> TelegramSharedDataManager::insertInputPeer(const InputPeer &peer, QByteArray *key)
{
    DECLARE_INSERT_FNC(InputPeer, peer)
}

TelegramSharedPointer<UserFullObject> TelegramSharedDataManager::insertUserFull(const UserFull &userFull, QByteArray *key)
{
    DECLARE_INSERT_FNC(UserFull, userFull)
}

TelegramSharedPointer<ChatFullObject> TelegramSharedDataManager::insertChatFull(const ChatFull &chatFull, QByteArray *key)
{
    DECLARE_INSERT_FNC(ChatFull, chatFull)
}

TelegramSharedPointer<StickerSetObject> TelegramSharedDataManager::insertStickerSet(const StickerSet &stickerSet, QByteArray *key)
{
    DECLARE_INSERT_FNC(StickerSet, stickerSet)
}

TelegramSharedPointer<DocumentObject> TelegramSharedDataManager::insertDocument(const Document &document, QByteArray *key)
{
    DECLARE_INSERT_FNC(Document, document)
}

TelegramSharedPointer<DialogObject> TelegramSharedDataManager::getDialog(const QByteArray &byte)
{
    DECLARE_GET_FNC(dialog)
}

TelegramSharedPointer<MessageObject> TelegramSharedDataManager::getMessage(const QByteArray &byte)
{
    DECLARE_GET_FNC(message)
}

TelegramSharedPointer<ChatObject> TelegramSharedDataManager::getChat(const QByteArray &byte)
{
    DECLARE_GET_FNC(chat)
}

TelegramSharedPointer<UserObject> TelegramSharedDataManager::getUser(const QByteArray &byte)
{
    DECLARE_GET_FNC(user)
}

TelegramSharedPointer<UserFullObject> TelegramSharedDataManager::getUserFull(const QByteArray &byte)
{
    DECLARE_GET_FNC(userFull)
}

TelegramSharedPointer<ChatFullObject> TelegramSharedDataManager::getChatFull(const QByteArray &byte)
{
    DECLARE_GET_FNC(chatFull)
}

TelegramSharedPointer<StickerSetObject> TelegramSharedDataManager::getStickerSet(const QByteArray &byte)
{
    DECLARE_GET_FNC(stickerSet)
}

TelegramSharedPointer<DocumentObject> TelegramSharedDataManager::getDocument(const QByteArray &byte)
{
    DECLARE_GET_FNC(document)
}

QList<DialogObject *> TelegramSharedDataManager::dialogs()
{
    DECLARE_GET_ALL_FNC(dialog)
}

QList<MessageObject *> TelegramSharedDataManager::messages()
{
    DECLARE_GET_ALL_FNC(message)
}

QList<ChatObject *> TelegramSharedDataManager::chats()
{
    DECLARE_GET_ALL_FNC(chat)
}

QList<UserObject *> TelegramSharedDataManager::users()
{
    DECLARE_GET_ALL_FNC(user)
}

TelegramSharedDataManager::~TelegramSharedDataManager()
{
    delete p;
}

