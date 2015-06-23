#ifndef MESSAGESSTICKERS
#define MESSAGESSTICKERS

#include <QtGlobal>
#include "stickerpack.h"
#include "document.h"

class MessagesStickers
{
public:

    enum MessagesStickersType {
       typeMessagesStickersNotModified = 0xf1749a22,
       typeMessagesStickers = 0x8a8ecd32
    };

    MessagesStickers(MessagesStickersType classType) :
        m_classType(classType) {}

    void setDocuments(QList<Document> documents) {
        m_documents = documents;
    }
    QList<Document> documents() const {
        return m_documents;
    }
    void setHash(QString hash) {
        m_hash = hash;
    }
    QString hash() const {
        return m_hash;
    }
    void setClassType(MessagesStickersType classType) {
        m_classType = classType;
    }
    MessagesStickersType classType() const {
        return m_classType;
    }

private:
    QString m_hash;
    QList<StickerPack> m_packs;
    QList<Document> m_documents;
    MessagesStickersType m_classType;
};

#endif // MESSAGESSTICKERS

