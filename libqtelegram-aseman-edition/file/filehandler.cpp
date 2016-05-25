#include "filehandler.h"
#include "secret/secretchat.h"
#include "secret/encrypter.h"

Q_LOGGING_CATEGORY(TG_FILE_FILEHANDLER, "tg.file.filehandler")

FileHandler::FileHandler(TelegramCore *core, TelegramApi *api, CryptoUtils *crypto, Settings *settings, DcProvider &dcProvider, SecretState &secretState, QObject *parent) :
    QObject(parent),
    mCore(core),
    mApi(api),
    mCrypto(crypto),
    mSettings(settings),
    mDcProvider(dcProvider),
    mSecretState(secretState) {

    connect(mApi, &TelegramApi::uploadSaveFilePartAnswer, this, &FileHandler::onUploadSaveFilePartResult);
    connect(mApi, &TelegramApi::uploadSaveBigFilePartAnswer, this, &FileHandler::onUploadSaveFilePartResult);
    connect(mApi, &TelegramApi::uploadGetFileAnswer, this, &FileHandler::onUploadGetFileAnswer);
    connect(mApi, &TelegramApi::uploadGetFileError, this, &FileHandler::onUploadGetFileError);
    connect(mApi, &TelegramApi::messagesSendMediaAnswer, this, &FileHandler::onMessagesSentMedia);
    connect(mApi, &TelegramApi::messagesSendEncryptedFileAnswer, this, &FileHandler::onMessagesSentEncryptedFile);
}

FileHandler::~FileHandler() {
    Q_FOREACH (UploadFileEngine::Ptr f, mUploadsMap.values()) {
        f.clear();
    }
    Q_FOREACH (FileOperation::Ptr op, mFileOperationsMap.values()) {
        op.clear();
    }
    Q_FOREACH (DownloadFile::Ptr f, mDownloadsMap.values()) {
        f.clear();
    }
}

qint64 FileHandler::uploadSendFile(FileOperation &op, const QString &fileName, const QByteArray &bytes, const QByteArray &thumbnailBytes, const QString &thumbnailName) {
    // create dedicated session to working dc for the main file
    DC *dc = mDcProvider.getWorkingDc();
    Session *session = mApi->fileSession(dc);
    if(!session)
        return 0;

    UploadFileEngine::Ptr f = UploadFileEngine::Ptr(new UploadFileEngine(session, mCrypto, UploadFileEngine::Main, bytes, this));
    bool encrypted = op.opType() == FileOperation::sendEncryptedFile;
    if (encrypted) {
        f->setEncrypted(true);
        f->setKey(op.key());
        f->setIv(op.iv());
    }
    f->setName(fileName);
    mUploadsMap.insert(f->id(), f);
    mFileOperationsMap.insert(f->id(), FileOperation::Ptr(&op));
    UploadFileEngine::Ptr firstFileToUpload = f;
    // if exists thumbnail, create its file object and assign id to main file thumbnail_id (and viceversa)
    // first send all thumbnail before sending main file
    if (!thumbnailBytes.isEmpty()) {
        // use same session for thumbnail and main file. In case a thumbnail exists, send it first
        UploadFileEngine::Ptr thumb = UploadFileEngine::Ptr(new UploadFileEngine(session, mCrypto, UploadFileEngine::Thumbnail, thumbnailBytes, this));
        f->setRelatedFileId(thumb->id());
        thumb->setRelatedFileId(f->id());
        thumbnailName.length() > 0 ? thumb->setName(thumbnailName) : thumb->setName(fileName);
        // insert the relationship fileId <-> thumb file
        mUploadsMap.insert(thumb->id(), thumb);
        firstFileToUpload = thumb;
    }

    if (session->state() == QAbstractSocket::ConnectedState) {
        uploadSendFileParts(*firstFileToUpload);
    } else {
        mInitialUploadsMap[session->sessionId()] << firstFileToUpload;
        connect(session, &Session::sessionReady, this, &FileHandler::onUploadSendFileSessionCreated);
        session->connectToServer();
    }

    return f->id();
}

qint64 FileHandler::uploadSendFile(FileOperation &op, const QString &filePath, const QString &thumbnailPath) {
    // create dedicated session to working dc for the main file
    DC *dc = mDcProvider.getWorkingDc();
    Session *session = mApi->fileSession(dc);
    if(!session)
        return 0;

    UploadFileEngine::Ptr f = UploadFileEngine::Ptr(new UploadFileEngine(session, mCrypto, UploadFileEngine::Main, filePath, this));
    bool encrypted = op.opType() == FileOperation::sendEncryptedFile;
    if (encrypted) {
        f->setEncrypted(true);
        f->setKey(op.key());
        f->setIv(op.iv());
    }
    mUploadsMap.insert(f->id(), f);
    mFileOperationsMap.insert(f->id(), FileOperation::Ptr(&op));
    UploadFileEngine::Ptr firstFileToUpload = f;
    // if exists thumbnail, create its file object and assign id to main file thumbnail_id (and viceversa)
    // first send all thumbnail before sending main file
    if (thumbnailPath.length() > 0) {
        // use same session for thumbnail and main file. In case a thumbnail exists, send it first
        UploadFileEngine::Ptr thumb = UploadFileEngine::Ptr(new UploadFileEngine(session, mCrypto, UploadFileEngine::Thumbnail, thumbnailPath, this));
        f->setRelatedFileId(thumb->id());
        thumb->setRelatedFileId(f->id());
        // insert the relationship fileId <-> thumb file
        mUploadsMap.insert(thumb->id(), thumb);
        firstFileToUpload = thumb;
    }

    if (session->state() == QAbstractSocket::ConnectedState) {
        uploadSendFileParts(*firstFileToUpload);
    } else {
        mInitialUploadsMap[session->sessionId()] << firstFileToUpload;
        connect(session, &Session::sessionReady, this, &FileHandler::onUploadSendFileSessionCreated);
        session->connectToServer();
    }

    return f->id();
}

void FileHandler::onUploadSendFileSessionCreated(DC *dc) {
    Q_UNUSED(dc)
    Session *session = qobject_cast<Session*>(sender());
    QList<UploadFileEngine::Ptr> sessionInitialFiles = mInitialUploadsMap.take(session->sessionId());
    Q_FOREACH (UploadFileEngine::Ptr f, sessionInitialFiles) {
        uploadSendFileParts(*f);
    }
}

qint64 FileHandler::uploadSendFileParts(UploadFileEngine &file) {
    // depending on size use one or other api method to upload every part
    qint32 partId = 0;

    while (file.hasMoreParts()) {
        QByteArray bytes = file.nextPart();
        if (file.length() > 10 * 1024 * 1024) {
            mApi->uploadSaveBigFilePart(file.id(), partId++, file.nParts(), bytes, file.id(), file.session());
        } else {
            mApi->uploadSaveFilePart(file.id(), partId++, bytes, file.id(), file.session());
        }
    }
    // return file id instead of every part msg id
    return file.id();
}

void FileHandler::onUploadSaveFilePartResult(qint64, bool, const QVariant &attachedData) {
    qint64 fileId = attachedData.toLongLong();
    UploadFileEngine::Ptr uploadFile = mUploadsMap.value(fileId);
    Q_ASSERT(!uploadFile.isNull());
    uploadFile->increaseUploadedParts();
    qCDebug(TG_FILE_FILEHANDLER) << "uploaded" << uploadFile->uploadedParts() << "parts out of" << uploadFile->nParts();

    // emit signal with upload progress
    qint32 length = uploadFile->length();
    qint32 uploaded = uploadFile->uploadedParts() * uploadFile->partLength();
    uploaded = uploaded > length ? length : uploaded;
    qint32 partId = uploadFile->uploadedParts() - 1;

    FileOperation::Ptr op = mFileOperationsMap.value(uploadFile->id());

    if (uploadFile->uploadedParts() == uploadFile->nParts()) {
        qCDebug(TG_FILE_FILEHANDLER) << "file upload finished for fileId" << fileId;

        // if finished a thumbnail, send the main file. Send media metadata if finished is the main file
        if (uploadFile->fileType() == UploadFileEngine::Thumbnail) {
            qint64 mainFileId = uploadFile->relatedFileId();
            UploadFileEngine::Ptr main = mUploadsMap.value(mainFileId);
            Q_ASSERT(!main.isNull());
            uploadSendFileParts(*main);
        } else {
            // compose InputFile to be added to metadata
            InputFile inputFile(InputFile::typeInputFile);
            if (uploadFile->length() > 10 * 1024 * 1024) {
                inputFile.setClassType(InputFile::typeInputFileBig);
            } else {
                inputFile.setMd5Checksum(QString(uploadFile->md5().toHex()));
            }
            inputFile.setId(fileId);
            inputFile.setParts(uploadFile->nParts());
            inputFile.setName(uploadFile->name());

            qint64 requestId = 0;
            // Read operation and execute internal api operation depending on file operation type
            mFileOperationsMap.remove(uploadFile->id());
            switch (op->opType()) {
                case FileOperation::sendMedia: {
                    InputMedia metadata = op->inputMedia();
                    metadata.setFile(inputFile);
                    // once filled input main file, check if there is a thumbnail and fill its data
                    if (uploadFile->relatedFileId()) {
                        UploadFileEngine::Ptr thumb = mUploadsMap.take(uploadFile->relatedFileId());
                        InputFile inputThumb(InputFile::typeInputFile);
                        if (thumb->length() > 10 * 1024 * 1024) {
                            inputThumb.setClassType(InputFile::typeInputFileBig);
                        } else {
                            inputThumb.setMd5Checksum(QString(thumb->md5().toHex()));
                        }
                        inputThumb.setId(thumb->id());
                        inputThumb.setParts(thumb->nParts());
                        inputThumb.setName(thumb->name());
                        metadata.setThumb(inputThumb);
                        thumb.clear();
                    }
                    // retrieve peer related to file, generate random id and send media metadata
                    InputPeer peer = op->peer();
                    qint64 randomId = op->randomId();
                    qint32 replyToMsgId = op->replyToMsgId();
                    requestId = mCore->messagesSendMedia(op->broadcast(), op->silent(), op->background(), peer, replyToMsgId,
                                                         metadata, randomId, op->replyMarkup(),
                                                         [=](TG_MESSAGES_SEND_MEDIA_CALLBACK){
                        UploadSendFile usf(error.null? UploadSendFile::typeUploadSendFileFinished :
                                                       UploadSendFile::typeUploadSendFileCanceled);
                        usf.setUpdates(result);
                        if(!op->resultCallbackIsNull())
                            op->resultCallback<UploadSendFile>()(msgId, usf, error);
                    }, op->timeOut());
                    break;
                }
                case FileOperation::editChatPhoto: {
                    InputChatPhoto metadata = op->inputChatPhoto();
                    metadata.setFile(inputFile);
                    qint32 chatId = op->chatId();
                    requestId = mCore->messagesEditChatPhoto(chatId, metadata, [=](TG_MESSAGES_EDIT_CHAT_PHOTO_CALLBACK){
                        UploadSendFile usf(error.null? UploadSendFile::typeUploadSendFileFinished :
                                                       UploadSendFile::typeUploadSendFileCanceled);
                        usf.setUpdates(result);
                        if(!op->resultCallbackIsNull())
                            op->resultCallback<UploadSendFile>()(msgId, usf, error);
                    }, op->timeOut());
                    break;
                }
                case FileOperation::uploadProfilePhoto: {
                    QString caption = op->caption();
                    InputGeoPoint geoPoint = op->geoPoint();
                    InputPhotoCrop crop = op->crop();
                    requestId = mCore->photosUploadProfilePhoto(inputFile, caption, geoPoint, crop, [=](TG_PHOTOS_UPLOAD_PROFILE_PHOTO_CALLBACK){
                        UploadSendPhoto usp(error.null? UploadSendPhoto::typeUploadSendPhotoFinished :
                                                        UploadSendPhoto::typeUploadSendPhotoCanceled);
                        usp.setPhoto(result);
                        if(!op->resultCallbackIsNull())
                            op->resultCallback<UploadSendPhoto>()(msgId, usp, error);
                    }, op->timeOut());
                    break;
                }
                case FileOperation::sendEncryptedFile: {
                    InputEncryptedChat inputEncryptedChat = op->inputEncryptedChat();
                    qint64 randomId = op->randomId();
                    DecryptedMessage decryptedMessage = op->decryptedMessage();

                    InputEncryptedFile inputEncryptedFile(InputEncryptedFile::typeInputEncryptedFileUploaded);
                    inputEncryptedFile.setId(fileId);
                    inputEncryptedFile.setParts(uploadFile->nParts());
                    inputEncryptedFile.setMd5Checksum(QString(uploadFile->md5().toHex()));

                    qint32 keyFingerprint = mCrypto->computeKeyFingerprint(op->key(), op->iv());
                    inputEncryptedFile.setKeyFingerprint(keyFingerprint);

                    SecretChat *secretChat = mSecretState.chats().value(inputEncryptedChat.chatId());
                    Encrypter encrypter(mSettings);
                    encrypter.setSecretChat(secretChat);
                    QByteArray data = encrypter.generateEncryptedData(decryptedMessage);
                    requestId = mCore->messagesSendEncryptedFile(inputEncryptedChat, randomId, data, inputEncryptedFile, [=](TG_MESSAGES_SEND_ENCRYPTED_FILE_CALLBACK){
                        UploadSendEncrypted use(error.null? UploadSendEncrypted::typeUploadSendEncryptedFinished :
                                                            UploadSendEncrypted::typeUploadSendEncryptedCanceled);
                        use.setMessage(result);
                        if(!op->resultCallbackIsNull())
                            op->resultCallback<UploadSendEncrypted>()(msgId, use, error);
                    }, op->timeOut());

                    secretChat->increaseOutSeqNo();
                    secretChat->appendToSequence(randomId);
                    mSecretState.save();
                    break;
                }
            }
            //correlate response of sendmedia with file id (that will be the id value to correlate the initial request)
            mFileIdsMap.insert(requestId, uploadFile->id());
            // delete main file from map after sending
            mUploadsMap.remove(fileId);
            // delete operation and main file pointers
            uploadFile.clear();
            op.clear();
        }
    } else if(!op->resultCallbackIsNull()) {
        UploadSendFile result(UploadSendFile::typeUploadSendFileProgress);
        result.setPartId(partId);
        result.setUploaded(uploaded);
        result.setTotalSize(length);

        switch (op->opType()) {
            case FileOperation::sendMedia:
            case FileOperation::editChatPhoto: {
                UploadSendFile usf(UploadSendFile::typeUploadSendFileProgress);
                usf.setPartId(partId);
                usf.setUploaded(uploaded);
                usf.setTotalSize(length);
                op->resultCallback<UploadSendFile>()(fileId, usf, TelegramCore::CallbackError());
                break;
            }
            case FileOperation::uploadProfilePhoto: {
                UploadSendPhoto usp(UploadSendPhoto::typeUploadSendPhotoProgress);
                usp.setPartId(partId);
                usp.setUploaded(uploaded);
                usp.setTotalSize(length);
                op->resultCallback<UploadSendPhoto>()(fileId, usp, TelegramCore::CallbackError());
                break;
            }
            case FileOperation::sendEncryptedFile: {
                UploadSendEncrypted use(UploadSendEncrypted::typeUploadSendEncryptedProgress);
                use.setPartId(partId);
                use.setUploaded(uploaded);
                use.setTotalSize(length);
                op->resultCallback<UploadSendEncrypted>()(fileId, use, TelegramCore::CallbackError());
                break;
            }
        }
    }

    Q_EMIT uploadSendFileAnswer(fileId, partId, uploaded, length);
}

qint64 FileHandler::uploadGetFile(const InputFileLocation &location, qint32 fileSize, qint32 dcNum, const QByteArray &key, const QByteArray &iv, qint32 timeOut) {
    // change of dc if received a dcNum
    DC *dc;
    dcNum ? dc = mDcProvider.getDc(dcNum) : dc = mDcProvider.getWorkingDc();
    if(!dc)
        return 0;
    Session *session = mApi->fileSession(dc);
    if(!session)
        return 0;

    DownloadFile::Ptr f = DownloadFile::Ptr(new DownloadFile(session, location, fileSize, this));
    f->setTimeOut(timeOut);
    if (location.classType() == InputFileLocation::typeInputEncryptedFileLocation) {
        f->setEncrypted(true);
        f->setKey(key);
        f->setIv(iv);
    }
    mActiveDownloadsMap.insert(f->id(), true);

    connect(session, &Session::updateMessageId, this, &FileHandler::onUpdateMessageId);

    switch (session->state()) {
    case QAbstractSocket::ConnectingState: {
        mInitialDownloadsMap[session->sessionId()] << f;
        break;
    }
    case QAbstractSocket::ConnectedState: {
        qint64 msgId = mApi->uploadGetFile(f->fileLocation(), 0, BLOCK, QVariant(), session);
        mDownloadsMap.insert(msgId, f);
        break;
    }
    default: {
        QList<DownloadFile::Ptr> sessionInitialFiles;
        sessionInitialFiles << f;
        mInitialDownloadsMap.insert(session->sessionId(), sessionInitialFiles);
        connect(session, &Session::sessionReady, this, &FileHandler::onUploadGetFileSessionCreated);
        session->connectToServer();
        break;
    }
    }

    // return file id as request id for apps
    return f->id();
}

void FileHandler::onUploadGetFileSessionCreated(DC *dc) {
    Q_UNUSED(dc)
    Session *session = qobject_cast<Session *>(sender());
    QList<DownloadFile::Ptr> sessionInitialFiles = mInitialDownloadsMap.take(session->sessionId());
    Q_FOREACH (DownloadFile::Ptr f, sessionInitialFiles) {
        qint64 msgId = mApi->uploadGetFile(f->fileLocation(), 0, BLOCK, QVariant(), session);
        mDownloadsMap.insert(msgId, f);
    }
}

void FileHandler::onUploadGetFileAnswer(qint64 msgId, const UploadFile &result, const QVariant &attachedData) {
    Q_UNUSED(attachedData)
    const StorageFileType &type = result.type();
    const qint32 mtime = result.mtime();
    QByteArray bytes = result.bytes();

    DownloadFile::Ptr f = mDownloadsMap.take(msgId);
    if(f.isNull())
        return;

    if (mCancelDownloadsMap.take(f->id())) {
        Q_EMIT uploadGetFileAnswer(f->id(), UploadGetFile(UploadGetFile::typeUploadGetFileCanceled));
        Q_EMIT uploadCancelFileAnswer(f->id(), true);
        mActiveDownloadsMap.remove(f->id());
        f.clear();
        return;
    }

    if (f->encrypted()) {
        mCrypto->decryptFilePart(bytes, f->key(), f->iv());
        // if decrypted bytes are more than the last part expected ones, remove surplus
        qint32 expectedMaxPartSize = f->expectedSize() - f->offset();
        if (bytes.length() > expectedMaxPartSize) {
            bytes.truncate(expectedMaxPartSize);
        }
    }

    // if 'managedDownloads' flag is set, store all temporal received data and return a unique event
    // when file downloading is finished. Emit an event for every part otherwise.
    if (mSettings->managedDownloads()) {
        // partial bytes appended to DownloadFile object
        f->appendBytes(bytes);

        qint32 expectedSize = f->expectedSize();
        if (expectedSize == 0 && (bytes.length() < BLOCK || bytes.length() == 0)) {
            expectedSize = f->offset();
        }

        if (expectedSize == 0 || f->bytes().length() < expectedSize) {
            qint64 newMsgId = mApi->uploadGetFile(f->fileLocation(), f->offset(), f->partLength(), QVariant(), f->session());
            mDownloadsMap.insert(newMsgId, f);
        } else {
            UploadGetFile result(UploadGetFile::typeUploadGetFileFinished);
            result.setType(type);
            result.setMtime(mtime);
            result.setBytes(f->bytes());
            result.setPartId(0);
            result.setDownloaded(f->length());
            result.setTotal(expectedSize);

            Q_EMIT uploadGetFileAnswer(f->id(), result); //emit signal of finished
            mActiveDownloadsMap.remove(f->id());
            f.clear();
        }
    } else {
        qint32 downloaded = f->partId() * f->partLength() + bytes.length();
        qint32 thisPartId = f->partId();
        qint32 expectedSize = f->expectedSize();

        f->incrementPartId();
        f->setOffset(f->offset() + bytes.length());

        if (expectedSize == 0 && (bytes.length() < BLOCK || bytes.length() == 0)) {
            expectedSize = f->offset();
        }

        qint64 fileId = f->id();

        UploadGetFile result(UploadGetFile::typeUploadGetFileFinished);
        result.setType(type);
        result.setMtime(mtime);
        result.setBytes(bytes);
        result.setPartId(thisPartId);
        result.setDownloaded(downloaded);
        result.setTotal(expectedSize);

        if (expectedSize == 0 || f->offset() < expectedSize) {
            result.setClassType(UploadGetFile::typeUploadGetFileProgress);
            qint64 newMsgId = mApi->uploadGetFile(f->fileLocation(), f->offset(), f->partLength(), QVariant(), f->session());
            mDownloadsMap.insert(newMsgId, f);
        } else {
            mActiveDownloadsMap.remove(f->id());
            f.clear();
        }

        Q_EMIT uploadGetFileAnswer(fileId, result);
    }
}

void FileHandler::onUploadGetFileError(qint64 id, qint32 errorCode, const QString &errorText, const QVariant &attachedData) {
    Q_UNUSED(attachedData)
    // check for error and resend authCheckPhone() request
    if (errorText.contains("_MIGRATE_")) {
        qint32 newDc = errorText.mid(errorText.lastIndexOf("_") + 1).toInt();
        qCDebug(TG_FILE_FILEHANDLER) << "file migrated to dc" << newDc;
        DC *dc = mDcProvider.getDc(newDc);
        DownloadFile::Ptr f = mDownloadsMap.take(id);
        if(!f)
            return;
        // release previous session
        f->session()->release();
        // get session against new dc
        Session *newDcSession = mApi->fileSession(dc);
        f->setSession(newDcSession);

        switch (newDcSession->state()) {
        case QAbstractSocket::ConnectingState: {
            mInitialDownloadsMap[newDcSession->sessionId()] << f;
            break;
        }
        case QAbstractSocket::ConnectedState: {
            qint64 msgId = mApi->uploadGetFile(f->fileLocation(), 0, BLOCK, QVariant(), newDcSession);
            mDownloadsMap.insert(msgId, f);
            break;
        }
        default: {
            QList<DownloadFile::Ptr> sessionInitialFiles;
            sessionInitialFiles << f;
            mInitialDownloadsMap.insert(newDcSession->sessionId(), sessionInitialFiles);
            connect(newDcSession, &Session::sessionReady, this, &FileHandler::onUploadGetFileSessionCreated);
            newDcSession->connectToServer();
            break;
        }
        }

    } else {
        Q_EMIT error(id, errorCode, errorText, __FUNCTION__);
    }
}

qint64 FileHandler::uploadCancelFile(qint64 fileId) {
    if (!mActiveDownloadsMap.value(fileId)) {
        // return signal of cancelled file, setting the 'cancelled' flag to false, only if the file is not into active
        // downloads. Otherwise, the signal will be emitted when received next part of the file
        Q_EMIT uploadCancelFileAnswer(fileId, false);
    } else {
        mCancelDownloadsMap.insert(fileId, true);
    }
    return fileId;
}

void FileHandler::onMessagesSentMedia(qint64 msgId, const UpdatesType &result, const QVariant &attachedData) {
    Q_UNUSED(attachedData)
    //recover correlated send media request id -> fileId
    qint64 fileId = mFileIdsMap.take(msgId);
    if(!fileId) // It's uploaded file
        return;

    Q_EMIT messagesSentMedia(fileId, result, QVariant());
}

void FileHandler::onMessagesSentEncryptedFile(qint64 msgId, const MessagesSentEncryptedMessage &result, const QVariant &attachedData) {
    Q_UNUSED(attachedData)
    const qint32 date = result.date();
    const EncryptedFile &encryptedFile = result.file();
    //recover correlated send media request id -> fileId
    qint64 fileId = mFileIdsMap.take(msgId);
    Q_ASSERT(fileId);
    Q_EMIT messagesSendEncryptedFileAnswer(fileId, date, encryptedFile);
}

void FileHandler::onUpdateMessageId(qint64 oldMsgId, qint64 newMsgId) {
    DownloadFile::Ptr file = mDownloadsMap.take(oldMsgId);
    if (!file.isNull()) {
        mDownloadsMap.insert(newMsgId, file);
    }
}
