#ifndef SAILORGRAMENUMS_H
#define SAILORGRAMENUMS_H

#include <QObject>

class SailorgramEnums : public QObject
{
    Q_OBJECT

    Q_ENUMS(MessageTypes)
    Q_ENUMS(ActionTypes)
    Q_ENUMS(SecretChatStates)

    public:
        enum MessageTypes { MessageTypeText,
                            MessageTypeDocument,
                            MessageTypeVideo,
                            MessageTypeAudio,
                            MessageTypeVenue,
                            MessageTypeWebPage,
                            MessageTypeGeo,
                            MessageTypeContact,
                            MessageTypePhoto,
                            MessageTypeSticker,
                            MessageTypeAnimated,
                            MessageTypeUnsupported };

        enum ActionTypes { ActionTypeEmpty,
                           ActionTypeChatCreate,
                           ActionTypeChatEditTitle,
                           ActionTypeChatEditPhoto,
                           ActionTypeChatDeletePhoto,
                           ActionTypeChatAddUser,
                           ActionTypeChatDeleteUser,
                           ActionTypeChatJoinedByLink,
                           ActionTypeChannelCreate,
                           ActionTypeChatMigrateTo,
                           ActionTypeChannelMigrateFrom,
                           ActionTypePinMessage,
                           ActionTypeHistoryClear };

        enum SecretChatStates { SecretChatStateInit,
                                SecretChatStateRequested,
                                SecretChatStateAccepted,
                                SecretChatStateCancelled };

    public:
        explicit SailorgramEnums(QObject *parent = 0);
};

#endif // SAILORGRAMENUMS_H
