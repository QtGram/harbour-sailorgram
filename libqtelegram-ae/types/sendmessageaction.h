#ifndef SENDMESSAGEACTION_H
#define SENDMESSAGEACTION_H

class SendMessageAction {

public:

    enum SendMessageActionType {
        typeSendMessageTypingAction = 0x16bf744e,
        typeSendMessageCancelAction = 0xfd5ec8f5,
        typeSendMessageRecordVideoAction = 0xa187d66f,
        typeSendMessageUploadVideoAction = 0x92042ff7,
        typeSendMessageRecordAudioAction = 0xd52f73f7,
        typeSendMessageUploadAudioAction = 0xe6ac8a6f,
        typeSendMessageUploadPhotoAction = 0x990a3c1a,
        typeSendMessageUploadDocumentAction = 0x8faee98e,
        typeSendMessageGeoLocationAction = 0x176f8ba1,
        typeSendMessageChooseContactAction = 0x628cbc6f
    };

    SendMessageAction(SendMessageActionType classType = typeSendMessageCancelAction) :
        mClassType(classType) {
    }

    void setClassType(SendMessageActionType classType) {
        mClassType = classType;
    }
    SendMessageActionType classType() const {
        return mClassType;
    }

private:
    SendMessageActionType mClassType;
};

#endif // SENDMESSAGEACTION_H
