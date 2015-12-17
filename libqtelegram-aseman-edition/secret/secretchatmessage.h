#ifndef SECRETCHATMESSAGE_H
#define SECRETCHATMESSAGE_H

#include "secret/decryptedmessage.h"
#include "telegram/types/encryptedfile.h"

class SecretChatMessage {

public:
    explicit SecretChatMessage() :
        mChatId(0),
        mTtl(0),
        mDate(0),
        mDecryptedMessage(DecryptedMessage::typeDecryptedMessage_level8),
        mAttachment(EncryptedFile::typeEncryptedFileEmpty) {}

    qint32 chatId() const { return mChatId; }
    qint32 ttl() const { return mTtl; }
    qint32 date() const { return mDate; }
    DecryptedMessage decryptedMessage() const { return mDecryptedMessage; }
    EncryptedFile attachment() const { return mAttachment; }
    void setChatId(qint32 chatId) { mChatId = chatId; }
    void setTtl(qint32 ttl) { mTtl = ttl; }
    void setDate(qint32 date) { mDate = date; }
    void setDecryptedMessage(const DecryptedMessage &decryptedMessage) { mDecryptedMessage = decryptedMessage; }
    void setAttachment(const EncryptedFile &attachment) { mAttachment = attachment; }

    bool isServiceMessage() const {
        return (mDecryptedMessage.classType() == DecryptedMessage::typeDecryptedMessageService ||
                mDecryptedMessage.classType() == DecryptedMessage::typeDecryptedMessageService_level8) ;
    }

private:
    qint32 mChatId;
    qint32 mTtl;
    qint32 mDate;
    DecryptedMessage mDecryptedMessage;
    EncryptedFile mAttachment;
};

#endif // SECRETCHATMESSAGE_H
