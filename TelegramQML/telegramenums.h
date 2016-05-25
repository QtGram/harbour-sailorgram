#ifndef TELEGRAMENUMS_H
#define TELEGRAMENUMS_H

#include "tqbaseobject.h"

#include <QObject>

class TelegramEnums : public QObject, public TqBaseObject
{
    Q_OBJECT
    Q_ENUMS(SendFileType)
    Q_ENUMS(MessageType)

public:
    enum SendFileType {
        SendFileTypeAutoDetect,
        SendFileTypeDocument,
        SendFileTypeSticker,
        SendFileTypeAnimated,
        SendFileTypeVideo,
        SendFileTypePhoto,
        SendFileTypeAudio
    };

    enum MessageType {
        TypeTextMessage,
        TypeDocumentMessage,
        TypeVideoMessage,
        TypeAudioMessage,
        TypeVenueMessage,
        TypeWebPageMessage,
        TypeGeoMessage,
        TypeContactMessage,
        TypeActionMessage,
        TypePhotoMessage,
        TypeStickerMessage,
        TypeAnimatedMessage,
        TypeUnsupportedMessage
    };

    TelegramEnums(QObject *parent = 0);
    ~TelegramEnums();

    static QStringList requiredProperties() {
        return QStringList();
    }
};

#endif // TELEGRAMENUMS_H
