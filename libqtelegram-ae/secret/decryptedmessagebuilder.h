#ifndef DECRYPTEDMESSAGEBUILDER_H
#define DECRYPTEDMESSAGEBUILDER_H

#include "secret/secretchat.h"

class DecryptedMessageBuilder
{
public:
    DecryptedMessageBuilder(qint32 layer);

    void setSecretChat(SecretChat *SecretChat);

    static QByteArray generateRandomBytes();

    DecryptedMessage buildDecryptedMessageForSendMessage(qint64 randomId, qint32 ttl, const QString &message);
    DecryptedMessage buildDecryptedMessageForNotifyLayer(qint64 randomId, qint32 layer);
    DecryptedMessage buildDecryptedMessageForTtl(qint64 randomId, qint32 ttl);
    DecryptedMessage buildDecryptedMessageForResend(qint64 randomId, qint32 startSeqNo, qint32 endSeqNo);
    DecryptedMessage buildDecryptedMessageForSendPhoto(qint64 randomId, qint32 ttl, const QByteArray &key, const QByteArray &iv, qint32 size = 0,
                                                       qint32 width = 0, qint32 height = 0,
                                                       const QByteArray &thumb = QByteArray(), qint32 thumbW = 90, qint32 thumbH = 90);
    DecryptedMessage buildDecryptedMessageForSendVideo(qint64 randomId, qint32 ttl, const QByteArray &key, const QByteArray &iv, qint32 size = 0,
                                                       const QString &mimeType = QString(),
                                                       qint32 duration = 0,  qint32 width = 0, qint32 height = 0,
                                                       const QByteArray &thumb = QByteArray(), qint32 thumbW = 90, qint32 thumbH = 90);
    DecryptedMessage buildDecryptedMessageForSendDocument(qint64 randomId, qint32 ttl, const QByteArray &key, const QByteArray &iv, qint32 size = 0,
                                                       const QString &fileName = QString(), const QString &mimeType = QString(),
                                                       const QByteArray &thumb = QByteArray(), qint32 thumbW = 90, qint32 thumbH = 90);

private:
    qint32 mLayer;
};

#endif // DECRYPTEDMESSAGEBUILDER_H
