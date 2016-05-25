#ifndef TELEGRAMTOOLS_H
#define TELEGRAMTOOLS_H

#include "telegramqml_global.h"
#include "telegramenums.h"
#include "telegram/types/types.h"

#include <QByteArray>
#include <functional>
#include <QJSValue>

class UserObject;
class MessageObject;
class TelegramTypeQObject;
class TELEGRAMQMLSHARED_EXPORT TelegramTools
{
public:
    static QByteArray identifier(const Peer &peer);
    static QByteArray identifier(qint32 peerType, qint32 peerId);
    static QByteArray identifier(const InputPeer &peer);
    static QByteArray identifier(const Dialog &dialog);
    static QByteArray identifier(const Dialog &dialog, qint32 messageId);
    static QByteArray identifier(const Peer &peer, qint32 messageId);
    static QByteArray identifier(const Message &message);
    static QByteArray identifier(const Chat &chat);
    static QByteArray identifier(const User &user);
    static QByteArray identifier(const UserFull &user);
    static QByteArray identifier(const ChatFull &chat);
    static QByteArray identifier(const StickerSet &stickerSet);
    static QByteArray identifier(const Document &document);
    static QByteArray identifier(class SecretChat *secretChat);

    static class InputPeer chatInputPeer(const Chat &chat);
    static class InputPeer userInputPeer(const User &user);
    static class InputPeer peerInputPeer(const Peer &peer, qint64 accessHash);
    static class InputPeer secretChatInputPeer(class SecretChat *secretChat);

    static class Peer chatPeer(const Chat &chat);
    static class Peer userPeer(const User &user);
    static class Peer dialogPeer(const Dialog &dialog);
    static class Peer messagePeer(const Message &message);
    static class Peer inputPeerPeer(const InputPeer &inputPeer);
    static class InputMedia mediaInputMedia(const MessageMedia &media);

    static qint64 generateRandomId();
    static QString convertErrorToText(const QString &error);

    static TelegramEnums::MessageType messageType(MessageObject *msg);
    static TelegramTypeQObject *objectRoot(TelegramTypeQObject *object);
    static void analizeUpdatesType(const UpdatesType &updates, class TelegramEngine *engine, std::function<void (const Update &update)> callback, const Message &sentMsg = Message::null);

    static QString userStatus(UserObject *user, std::function<QString (const QDateTime &)> dateConversationMethod);
};

#endif // TELEGRAMTOOLS_H
