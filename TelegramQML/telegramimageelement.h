#ifndef TELEGRAMIMAGEELEMENT_H
#define TELEGRAMIMAGEELEMENT_H

#include <QQuickItem>
#include <QSizeF>
#include <telegram/objects/telegramtypeqobject.h>

#include "telegramqml_global.h"
#include "telegramtools.h"
#include "tqbaseobject.cpp"

class TelegramEngine;
class TelegramImageElementPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramImageElement : public QQuickItem, public TqBaseObject
{
    Q_OBJECT
    Q_PROPERTY(TelegramTypeQObject* source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(TelegramEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)
    Q_PROPERTY(QString qtQuickVersion READ qtQuickVersion WRITE setQtQuickVersion NOTIFY qtQuickVersionChanged)
    Q_PROPERTY(qint32 fileSize READ fileSize NOTIFY fileSizeChanged)
    Q_PROPERTY(qint32 downloadedSize READ downloadedSize NOTIFY downloadedSizeChanged)
    Q_PROPERTY(bool downloading READ downloading NOTIFY downloadingChanged)
    Q_PROPERTY(bool downloaded READ downloaded NOTIFY downloadedChanged)
    Q_PROPERTY(bool thumbnailDownloaded READ thumbnailDownloaded NOTIFY thumbnailDownloadedChanged)
    Q_PROPERTY(QUrl destination READ destination NOTIFY destinationChanged)
    Q_PROPERTY(QUrl thumbnail READ thumbnail NOTIFY thumbnailChanged)
    Q_PROPERTY(QUrl currentImage READ currentImage NOTIFY currentImageChanged)
    Q_PROPERTY(QSizeF imageSize READ imageSize NOTIFY imageSizeChanged)
    Q_PROPERTY(QString errorText READ errorText NOTIFY errorChanged)
    Q_PROPERTY(qint32 errorCode READ errorCode NOTIFY errorChanged)

    Q_PROPERTY(bool asynchronous READ asynchronous WRITE setAsynchronous NOTIFY asynchronousChanged)
    Q_PROPERTY(bool autoTransform READ autoTransform WRITE setAutoTransform NOTIFY autoTransformChanged)
    Q_PROPERTY(bool cache READ cache WRITE setCache NOTIFY cacheChanged)
    Q_PROPERTY(int fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_PROPERTY(int horizontalAlignment READ horizontalAlignment WRITE setHorizontalAlignment NOTIFY horizontalAlignmentChanged)
    Q_PROPERTY(bool mipmap READ mipmap WRITE setMipmap NOTIFY mipmapChanged)
    Q_PROPERTY(bool mirror READ mirror WRITE setMirror NOTIFY mirrorChanged)
    Q_PROPERTY(qreal paintedHeight READ paintedHeight NOTIFY paintedHeightChanged)
    Q_PROPERTY(qreal paintedWidth READ paintedWidth NOTIFY paintedWidthChanged)
    Q_PROPERTY(bool smooth READ smooth WRITE setSmooth NOTIFY smoothChanged)
    Q_PROPERTY(QSizeF sourceSize READ sourceSize WRITE setSourceSize NOTIFY sourceSizeChanged)
    Q_PROPERTY(int verticalAlignment READ verticalAlignment WRITE setVerticalAlignment NOTIFY verticalAlignmentChanged)

public:
    TelegramImageElement(QQuickItem *parent = 0);
    ~TelegramImageElement();

    void setSource(TelegramTypeQObject *object);
    TelegramTypeQObject *source() const;

    void setEngine(TelegramEngine *engine);
    TelegramEngine *engine() const;

    void setQtQuickVersion(const QString& version);
    const QString& qtQuickVersion() const;

    bool asynchronous();
    void setAsynchronous(bool asynchronous);

    bool autoTransform();
    void setAutoTransform(bool autoTransform);

    bool cache();
    void setCache(bool cache);

    int fillMode();
    void setFillMode(int fillMode);

    int horizontalAlignment();
    void setHorizontalAlignment(int horizontalAlignment);

    bool mipmap();
    void setMipmap(bool mipmap);

    bool mirror();
    void setMirror(bool mirror);

    qreal paintedHeight();
    qreal paintedWidth();

    bool smooth();
    void setSmooth(bool smooth);

    QSizeF sourceSize();
    void setSourceSize(const QSizeF &sourceSize);

    int verticalAlignment();
    void setVerticalAlignment(int verticalAlignment);

    QSizeF imageSize() const;

    qint32 fileSize() const;
    qint32 downloadedSize() const;
    bool downloading() const;
    bool downloaded() const;
    bool thumbnailDownloaded() const;
    QUrl destination() const;
    QUrl thumbnail() const;
    QUrl currentImage() const;

    QString errorText() const;
    qint32 errorCode() const;

    static QStringList requiredProperties();

Q_SIGNALS:
    void sourceChanged();
    void engineChanged();
    void qtQuickVersionChanged();
    void imageSizeChanged();
    void fileSizeChanged();
    void downloadedSizeChanged();
    void downloadingChanged();
    void destinationChanged();
    void thumbnailChanged();
    void currentImageChanged();
    void asynchronousChanged();
    void autoTransformChanged();
    void cacheChanged();
    void fillModeChanged();
    void horizontalAlignmentChanged();
    void mipmapChanged();
    void mirrorChanged();
    void paintedHeightChanged();
    void paintedWidthChanged();
    void smoothChanged();
    void sourceSizeChanged();
    void verticalAlignmentChanged();
    void downloadedChanged();
    void thumbnailDownloadedChanged();
    void errorChanged();

public Q_SLOTS:
    bool download();
    bool check();
    void stop();

private:
    void initImage();

protected:
    void setImage(const QString &image);
    virtual void destinationChangeEvent();
    virtual void thumbnailChangeEvent();

private:
    TelegramImageElementPrivate *p;
};

#endif // TELEGRAMIMAGEELEMENT_H
