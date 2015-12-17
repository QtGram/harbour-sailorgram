#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QObject>
#include <QSharedPointer>
#include "uploadfileengine.h"
#include "fileoperation.h"
#include "downloadfile.h"
#include "core/dcprovider.h"
#include "core/api.h"
#include "secret/secretstate.h"

Q_DECLARE_LOGGING_CATEGORY(TG_FILE_FILEHANDLER)

class FileHandler : public QObject
{
    Q_OBJECT
public:
    typedef QSharedPointer<FileHandler> Ptr;

    explicit FileHandler(Api* api, CryptoUtils *crypto, Settings *settings, DcProvider &dcProvider, SecretState &secretState, QObject *parent = 0);
    ~FileHandler();

    qint64 uploadSendFile(FileOperation &op, const QString &fileName, const QByteArray &bytes, const QByteArray &thumbnailBytes = 0, const QString &thumbnailName = QString::null);
    qint64 uploadSendFile(FileOperation &op, const QString &filePath, const QString &thumbnailPath = QString::null);
    qint64 uploadGetFile(const InputFileLocation &file, qint32 fileSize, qint32 dc = 0, const QByteArray &key = QByteArray(), const QByteArray &iv = QByteArray());
    qint64 uploadCancelFile(qint64 fileId);

Q_SIGNALS:
    void uploadSendFileAnswer(qint64 fileId, qint32 partId, qint32 uploaded, qint32 totalSize);
    void uploadGetFileAnswer(qint64 fileId, const StorageFileType &type, qint32 mtime, const QByteArray &bytes, qint32 partId, qint32 downloaded, qint32 total);
    void uploadCancelFileAnswer(qint64 fileId, bool cancelled);
    void error(qint64 id, qint32 errorCode, const QString &errorText);

    void messagesSentMedia(qint64 fileId, const UpdatesType &updates);
    void messagesSendEncryptedFileAnswer(qint64 id, qint32 date, const EncryptedFile &encryptedFile = EncryptedFile());

private:
    Api *mApi;
    CryptoUtils *mCrypto;
    Settings *mSettings;
    DcProvider &mDcProvider;
    SecretState &mSecretState;

    /* File Sending */
    // fileId -> physical file details (length, bytes, parts, every part details, etc..)
    QMap<qint64, UploadFileEngine::Ptr> mUploadsMap;
    // mainFileId -> operation additional data to be done with the file (peer, operation type)
    QMap<qint64, FileOperation::Ptr> mFileOperationsMap;
    // sessionId -> list of files waiting for session connection to be ready for being uploaded
    QMap<qint64, QList<UploadFileEngine::Ptr> > mInitialUploadsMap;
    // finally, when sending media by using telegram api messagesSendMedia, we need to correlate
    // requestId -> fileId, cos fileId is what we returned in request and the identifier we need
    // to answer in final response
    QMap<qint64, qint64> mFileIdsMap;

    qint64 uploadSendFileParts(UploadFileEngine &file);

    /* File receiving */
    // requestId -> physical downloading file details (DownloadFile structure includes fileId)
    QMap<qint64, DownloadFile::Ptr> mDownloadsMap;
    // sessionId -> list of files waiting for session connection to be ready for being downloaded
    QMap<qint64, QList<DownloadFile::Ptr> > mInitialDownloadsMap;
    // fileId -> true if the file download is in progress. An entry does not exists for that fileId if not downloading
    QMap<qint64, bool> mActiveDownloadsMap;
    // fileId -> true if requested a download cancel for that fileId
    QMap<qint64, bool> mCancelDownloadsMap;

private Q_SLOTS:
    void onUploadSendFileSessionCreated();
    void onUploadSaveFilePartResult(qint64 msgId, qint64 fileId, bool ok);
    void onUploadGetFileSessionCreated();
    void onUploadGetFileAnswer(qint64 msgId, const StorageFileType &type, qint32 mtime, QByteArray bytes);
    void onUploadGetFileError(qint64 id, qint32 errorCode, const QString &errorText);

    void onMessagesSentMedia(qint64 id, const UpdatesType &updates);
    void onMessagesSentEncryptedFile(qint64, qint32 date, const EncryptedFile &encryptedFile = EncryptedFile());

    void onUpdateMessageId(qint64 oldMsgId, qint64 newMsgId);
};

#endif // FILEHANDLER_H
