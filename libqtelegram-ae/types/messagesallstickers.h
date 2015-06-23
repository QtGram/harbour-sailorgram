#ifndef MESSAGESALLSTICKERS
#define MESSAGESALLSTICKERS

#include <QtGlobal>
#include "stickerpack.h"
#include "document.h"

class MessagesAllStickers
{
public:

    enum MessagesAllStickersType {
       typeMessagesAllStickersNotModified = 0xe86602c3,
       typeMessagesAllStickers = 0xdcef3102
    };

    MessagesAllStickers(MessagesAllStickersType classType) :
        m_classType(classType) {}

    void setDocuments(QList<Document> documents) {
        m_documents = documents;
    }
    QList<Document> documents() const {
        return m_documents;
    }
    void setPacks(QList<StickerPack> packs) {
        m_packs = packs;
    }
    QList<StickerPack> packs() const {
        return m_packs;
    }
    void setHash(QString hash) {
        m_hash = hash;
    }
    QString hash() const {
        return m_hash;
    }
    void setClassType(MessagesAllStickersType classType) {
        m_classType = classType;
    }
    MessagesAllStickersType classType() const {
        return m_classType;
    }

private:
    QString m_hash;
    QList<StickerPack> m_packs;
    QList<Document> m_documents;
    MessagesAllStickersType m_classType;
};

#endif // MESSAGESALLSTICKERS

