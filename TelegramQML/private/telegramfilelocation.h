#ifndef TELEGRAMFILELOCATION_H
#define TELEGRAMFILELOCATION_H

#include "telegramqml_macros.h"
#include "tqobject.h"

#include <QSizeF>

class InputFileLocationObject;
class TelegramEngine;
class TelegramFileLocationPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramFileLocation : public TqObject
{
    Q_OBJECT
    Q_PROPERTY(qint32 dcId READ dcId WRITE setDcId NOTIFY dcIdChanged)
    Q_PROPERTY(qint32 localId READ localId WRITE setLocalId NOTIFY localIdChanged)
    Q_PROPERTY(qint64 secret READ secret WRITE setSecret NOTIFY secretChanged)
    Q_PROPERTY(qint64 volumeId READ volumeId WRITE setVolumeId NOTIFY volumeIdChanged)
    Q_PROPERTY(qint64 accessHash READ accessHash WRITE setAccessHash NOTIFY accessHashChanged)
    Q_PROPERTY(qint64 id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(qint32 size READ size  WRITE setSize  NOTIFY sizeChanged)
    Q_PROPERTY(QSizeF imageSize READ imageSize WRITE setImageSize NOTIFY imageSizeChanged)
    Q_PROPERTY(qint32 downloadedSize READ downloadedSize NOTIFY downloadedSizeChanged)
    Q_PROPERTY(qint32 downloadTotal READ downloadTotal NOTIFY downloadTotalChanged)
    Q_PROPERTY(bool downloading READ downloading NOTIFY downloadingChanged)
    Q_PROPERTY(TelegramEngine* engine READ engine NOTIFY engineChanged)
    Q_PROPERTY(int classType READ classType WRITE setClassType NOTIFY classTypeChanged)
    Q_PROPERTY(InputFileLocationObject* location READ location NOTIFY locationChanged)
    Q_PROPERTY(QString destination READ destination NOTIFY destinationChanged)

public:
    TelegramFileLocation(TelegramEngine *engine);
    virtual ~TelegramFileLocation();

    qint32 dcId() const;
    void setDcId(const qint32 &dcId);

    qint32 localId() const;
    void setLocalId(const qint32 &localId);

    qint64 secret() const;
    void setSecret(const qint64 &secret);

    qint64 volumeId() const;
    void setVolumeId(const qint64 &volumeId);

    qint64 accessHash() const;
    void setAccessHash(const qint64 &accessHash);

    qint64 id() const;
    void setId(const qint64 &id);

    qint32 size() const;
    void setSize(const qint32 &size);

    QSizeF imageSize() const;
    void setImageSize(const QSizeF &imageSize);

    qint32 downloadedSize() const;
    qint32 downloadTotal() const;
    bool downloading() const;

    TelegramEngine *engine() const;

    int classType() const;
    void setClassType(int classType);

    InputFileLocationObject *location() const;
    QString destination() const;

    static QStringList requiredProperties();

Q_SIGNALS:
    void dcIdChanged();
    void localIdChanged();
    void secretChanged();
    void volumeIdChanged();
    void accessHashChanged();
    void idChanged();
    void sizeChanged();
    void imageSizeChanged();
    void downloadedSizeChanged();
    void downloadTotalChanged();
    void downloadingChanged();
    void engineChanged();
    void classTypeChanged();
    void locationChanged();
    void destinationChanged();

    void finished();

public Q_SLOTS:
    bool download();
    bool check();
    void stop();

protected:
    QString getLocation(bool *uploading = 0) const;

private:
    void setDownloadedSize(qint32 size);
    void setDownloadTotal(qint32 total);
    void setDownloading(bool downloading);
    void setDestination(const QString &destination);

private:
    TelegramFileLocationPrivate *p;
};

#endif // TELEGRAMFILELOCATION_H
