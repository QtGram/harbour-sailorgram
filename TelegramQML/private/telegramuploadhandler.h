#ifndef TELEGRAMUPLOADHANDLER_H
#define TELEGRAMUPLOADHANDLER_H

#include "telegramqml_macros.h"
#include "telegramqml_global.h"
#include "tqobject.h"

#include <QUrl>

class ReplyMarkupObject;
class Message;
class MessageObject;
class TelegramTypeQObject;
class TelegramEngine;
class InputPeerObject;
class TelegramUploadHandlerPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramUploadHandler : public TqObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)
    Q_PROPERTY(InputPeerObject* currentPeer READ currentPeer WRITE setCurrentPeer NOTIFY currentPeerChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(int sendFileType READ sendFileType WRITE setSendFileType NOTIFY sendFileTypeChanged)
    Q_PROPERTY(int status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(MessageObject* target READ target NOTIFY targetChanged)
    Q_PROPERTY(MessageObject* result READ result NOTIFY resultChanged)
    Q_PROPERTY(MessageObject* replyTo READ replyTo WRITE setReplyTo NOTIFY replyToChanged)
    Q_PROPERTY(ReplyMarkupObject* replyMarkup READ replyMarkup WRITE setReplyMarkup NOTIFY replyMarkupChanged)
    Q_PROPERTY(bool silent READ silent WRITE setSilent NOTIFY silentChanged)
    Q_PROPERTY(bool noWebpage READ noWebpage WRITE setNoWebpage NOTIFY noWebpageChanged)
    Q_PROPERTY(bool supergroup READ supergroup WRITE setSupergroup NOTIFY supergroupChanged)
    Q_PROPERTY(qint32 transfaredSize READ transfaredSize NOTIFY transfaredSizeChanged)
    Q_PROPERTY(qint32 totalSize READ totalSize NOTIFY totalSizeChanged)
    Q_PROPERTY(QString alt READ alt WRITE setAlt NOTIFY altChanged)
    Q_PROPERTY(qint32 duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(qint32 h READ h WRITE setH NOTIFY hChanged)
    Q_PROPERTY(QString performer READ performer WRITE setPerformer NOTIFY performerChanged)
    Q_PROPERTY(InputStickerSet stickerset READ stickerset WRITE setStickerset NOTIFY stickersetChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QUrl thumbnail READ thumbnail WRITE setThumbnail NOTIFY thumbnailChanged)
    Q_PROPERTY(bool voice READ voice WRITE setVoice NOTIFY voiceChanged)
    Q_PROPERTY(qint32 w READ w WRITE setW NOTIFY wChanged)
    Q_PROPERTY(QByteArray fakeKey READ fakeKey NOTIFY fakeKeyChanged)

public:
    enum Status {
        StatusNone,
        StatusUploading,
        StatusSending,
        StatusError,
        StatusDone
    };

    TelegramUploadHandler(QObject *parent = 0);
    ~TelegramUploadHandler();

    void setEngine(TelegramEngine *engine);
    TelegramEngine *engine() const;

    void setCurrentPeer(InputPeerObject *currentPeer);
    InputPeerObject *currentPeer() const;

    void setText(const QString &text);
    QString text() const;

    void setFile(const QString &file);
    QString file() const;

    void setAlt(const QString &alt);
    QString alt() const;

    void setDuration(qint32 duration);
    qint32 duration() const;

    void setFileName(const QString &fileName);
    QString fileName() const;

    void setH(qint32 h);
    qint32 h() const;

    void setPerformer(const QString &performer);
    QString performer() const;

    void setStickerset(const InputStickerSet &stickerset);
    InputStickerSet stickerset() const;

    void setTitle(const QString &title);
    QString title() const;

    void setVoice(bool voice);
    bool voice() const;

    void setW(qint32 w);
    qint32 w() const;

    void setSilent(bool silent);
    bool silent() const;

    void setSupergroup(bool supergroup);
    bool supergroup() const;

    void setNoWebpage(bool noWebpage);
    bool noWebpage() const;

    void setSendFileType(int sendFileType);
    int sendFileType() const;

    void setReplyTo(MessageObject *message);
    MessageObject *replyTo() const;

    void setReplyMarkup(ReplyMarkupObject *markup);
    ReplyMarkupObject *replyMarkup() const;

    int status() const;

    void setTarget(MessageObject *object);
    MessageObject *target() const;

    MessageObject *result() const;
    void setResult(const Message &message);
    QByteArray fakeKey() const;

    qint32 transfaredSize() const;
    qint32 totalSize() const;
    QUrl thumbnail() const;

    static QList<TelegramUploadHandler*> getItems(TelegramEngine *engine, InputPeerObject *peer);
    QList<TelegramUploadHandler*> getItems();

    static QStringList requiredProperties();

public Q_SLOTS:
    bool send();
    void cancel();

Q_SIGNALS:
    void engineChanged();
    void currentPeerChanged();
    void textChanged();
    void fileChanged();
    void altChanged();
    void durationChanged();
    void fileNameChanged();
    void hChanged();
    void supergroupChanged();
    void performerChanged();
    void stickersetChanged();
    void titleChanged();
    void voiceChanged();
    void wChanged();
    void silentChanged();
    void fakeKeyChanged();
    void noWebpageChanged();
    void sendFileTypeChanged();
    void statusChanged();
    void targetChanged();
    void replyToChanged();
    void replyMarkupChanged();
    void resultChanged();
    void transfaredSizeChanged();
    void totalSizeChanged();
    void thumbnailChanged();
    void sent();

protected:
    bool sendMessage();
    bool sendFile();

    QByteArray identifier() const;
    Message createNewMessage();

    void setStatus(int status);
    void setFakeKey(const QByteArray &fakeKey);
    void setTransfaredSize(qint32 size);
    void setTotalSize(qint32 size);
    void setThumbnail(const QUrl &url);

    void onUpdate(const class UpdatesType &update, const Message &sentMsg = Message::null);
    void insertUpdate(const class Update &update);

private:
    bool sendDocument(const MessageMedia &media);
    void sendDocument_step2(int type, const QString &thumbnail);

private:
    TelegramUploadHandlerPrivate *p;
};

#endif // TELEGRAMUPLOADHANDLER_H
