#include "decryptedmessagebuilder.h"
#include "util/utils.h"

DecryptedMessageBuilder::DecryptedMessageBuilder(qint32 layer) : mLayer(layer) {
}

QByteArray DecryptedMessageBuilder::generateRandomBytes() {
    qint32 n = 15 + 4 * (lrand48() % 3);
    QScopedArrayPointer<char> rnd(new char[n]);
    Utils::randomBytes(rnd.data(), n);
    QByteArray randomBytes(rnd.data());
    return randomBytes;
}

DecryptedMessage DecryptedMessageBuilder::buildDecryptedMessageForSendMessage(qint64 randomId, qint32 ttl, const QString &message) {

    DecryptedMessage::DecryptedMessageType type = mLayer < 17 ?
                DecryptedMessage::typeDecryptedMessage_level8 : DecryptedMessage::typeDecryptedMessage;

    DecryptedMessage decryptedMessage(type);
    decryptedMessage.setRandomId(randomId);
    if (mLayer < 17) {
        decryptedMessage.setRandomBytes(generateRandomBytes());
    } else {
        decryptedMessage.setTtl(ttl);
    }
    decryptedMessage.setMessage(message);

    return decryptedMessage;
}

DecryptedMessage DecryptedMessageBuilder::buildDecryptedMessageForSendVideo(qint64 randomId, qint32 ttl, const QByteArray &key, const QByteArray &iv, qint32 size,
                                                        const QString &mimeType,
                                                        qint32 duration, qint32 width, qint32 height,
                                                        const QByteArray &thumb, qint32 thumbW, qint32 thumbH) {

    DecryptedMessageMedia::DecryptedMessageMediaType mediaType = mLayer < 17 ?
                DecryptedMessageMedia::typeDecryptedMessageMediaVideo_layer8 :
                DecryptedMessageMedia::typeDecryptedMessageMediaVideo;

    DecryptedMessageMedia media(mediaType);
    media.setThumb(thumb);
    media.setThumbW(thumbW);
    media.setThumbH(thumbH);
    media.setDuration(duration);
    media.setMimeType(mimeType);
    media.setW(width);
    media.setH(height);
    media.setSize(size);
    media.setKey(key);
    media.setIv(iv);

    DecryptedMessage::DecryptedMessageType type = mLayer < 17 ?
                DecryptedMessage::typeDecryptedMessage_level8 : DecryptedMessage::typeDecryptedMessage;

    DecryptedMessage decryptedMessage(type);
    decryptedMessage.setRandomId(randomId);
    if (mLayer < 17) {
        decryptedMessage.setRandomBytes(generateRandomBytes());
    } else {
        decryptedMessage.setTtl(ttl);
    }
    decryptedMessage.setMedia(media);

    return decryptedMessage;
}

DecryptedMessage DecryptedMessageBuilder::buildDecryptedMessageForSendPhoto(qint64 randomId, qint32 ttl, const QByteArray &key, const QByteArray &iv, qint32 size,
                                                                            qint32 width, qint32 height,
                                                                            const QByteArray &thumb, qint32 thumbW, qint32 thumbH) {

    DecryptedMessageMedia media(DecryptedMessageMedia::typeDecryptedMessageMediaPhoto);
    media.setThumb(thumb);
    media.setThumbW(thumbW);
    media.setThumbH(thumbH);
    media.setW(width);
    media.setH(height);
    media.setSize(size);
    media.setKey(key);
    media.setIv(iv);

    DecryptedMessage::DecryptedMessageType type = mLayer < 17 ?
                DecryptedMessage::typeDecryptedMessage_level8 : DecryptedMessage::typeDecryptedMessage;

    DecryptedMessage decryptedMessage(type);
    decryptedMessage.setRandomId(randomId);
    if (mLayer < 17) {
        decryptedMessage.setRandomBytes(generateRandomBytes());
    } else {
        decryptedMessage.setTtl(ttl);
    }
    decryptedMessage.setMedia(media);

    return decryptedMessage;
}


DecryptedMessage DecryptedMessageBuilder::buildDecryptedMessageForSendDocument(qint64 randomId, qint32 ttl, const QByteArray &key, const QByteArray &iv, qint32 size,
                                                                               const QString &fileName, const QString &mimeType,
                                                                               const QByteArray &thumb, qint32 thumbW, qint32 thumbH) {

    DecryptedMessageMedia media(DecryptedMessageMedia::typeDecryptedMessageMediaDocument);
    media.setThumb(thumb);
    media.setThumbW(thumbW);
    media.setThumbH(thumbH);
    media.setFileName(fileName);
    media.setMimeType(mimeType);
    media.setSize(size);
    media.setKey(key);
    media.setIv(iv);

    DecryptedMessage::DecryptedMessageType type = mLayer < 17 ?
                DecryptedMessage::typeDecryptedMessage_level8 : DecryptedMessage::typeDecryptedMessage;

    DecryptedMessage decryptedMessage(type);
    decryptedMessage.setRandomId(randomId);
    if (mLayer < 17) {
        decryptedMessage.setRandomBytes(generateRandomBytes());
    } else {
        decryptedMessage.setTtl(ttl);
    }
    decryptedMessage.setMedia(media);

    return decryptedMessage;
}

DecryptedMessage DecryptedMessageBuilder::buildDecryptedMessageForNotifyLayer(qint64 randomId, qint32 layer) {

    DecryptedMessage::DecryptedMessageType type = mLayer < 17 ?
                DecryptedMessage::typeDecryptedMessageService_level8 : DecryptedMessage::typeDecryptedMessageService;

    DecryptedMessage decryptedMessage(type);
    decryptedMessage.setRandomId(randomId);
    if (mLayer < 17) {
        decryptedMessage.setRandomBytes(generateRandomBytes());
    }
    DecryptedMessageAction action(DecryptedMessageAction::typeDecryptedMessageActionNotifyLayer);
    action.setLayer(layer);
    decryptedMessage.setAction(action);

    return decryptedMessage;
}

DecryptedMessage DecryptedMessageBuilder::buildDecryptedMessageForTtl(qint64 randomId, qint32 ttl) {

    DecryptedMessage::DecryptedMessageType type = mLayer < 17 ?
                DecryptedMessage::typeDecryptedMessageService_level8 : DecryptedMessage::typeDecryptedMessageService;

    DecryptedMessage decryptedMessage(type);
    decryptedMessage.setRandomId(randomId);
    if (mLayer < 17) {
        decryptedMessage.setRandomBytes(generateRandomBytes());
    }
    DecryptedMessageAction action(DecryptedMessageAction::typeDecryptedMessageActionSetMessageTTL);
    action.setTtlSeconds(ttl);
    decryptedMessage.setAction(action);

    return decryptedMessage;
}

DecryptedMessage DecryptedMessageBuilder::buildDecryptedMessageForResend(qint64 randomId, qint32 startSeqNo, qint32 endSeqNo) {

    DecryptedMessage::DecryptedMessageType type = mLayer < 17 ?
                DecryptedMessage::typeDecryptedMessageService_level8 : DecryptedMessage::typeDecryptedMessageService;

    DecryptedMessage decryptedMessage(type);
    decryptedMessage.setRandomId(randomId);
    if (mLayer < 17) {
        decryptedMessage.setRandomBytes(generateRandomBytes());
    }
    DecryptedMessageAction action(DecryptedMessageAction::typeDecryptedMessageActionResend);
    action.setStartSeqNo(startSeqNo);
    action.setEndSeqNo(endSeqNo);
    decryptedMessage.setAction(action);

    return decryptedMessage;
}
