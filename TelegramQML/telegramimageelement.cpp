#define READ_PROPERTY(NAME, TYPE) \
    initImage(); \
    return p->image->property(#NAME).value<TYPE>();

#define SET_PROPERTY(NAME, TYPE) \
    initImage(); \
    p->image->setProperty(#NAME, QVariant::fromValue<TYPE>(NAME));

#include "telegramimageelement.h"
#include "telegramengine.h"
#include "private/telegramdownloadhandler.h"

#include <telegram/objects/typeobjects.h>

#include <QDebug>
#include <QtQml>
#include <QPointer>
#include <QMimeDatabase>
#include <QMimeType>
#include <QFileInfo>
#include <QImageReader>

class TelegramImageElementPrivate
{
public:
    TelegramDownloadHandler *handler;
    QQuickItem *image;
    QMimeDatabase mdb;
    QSizeF imageSize;
    QString qtQuickVersion;
};

TelegramImageElement::TelegramImageElement(QQuickItem *parent) :
    QQuickItem(parent)
{
    p = new TelegramImageElementPrivate;
    p->image = 0;
    p->qtQuickVersion = "2.5";

    p->handler = new TelegramDownloadHandler(this);

    connect(p->handler, &TelegramDownloadHandler::sourceChanged       , this, &TelegramImageElement::sourceChanged       );
    connect(p->handler, &TelegramDownloadHandler::engineChanged       , this, &TelegramImageElement::engineChanged       );
    connect(p->handler, &TelegramDownloadHandler::sizeChanged         , this, &TelegramImageElement::fileSizeChanged     );
    connect(p->handler, &TelegramDownloadHandler::downloadTotalChanged, this, &TelegramImageElement::fileSizeChanged     );
    connect(p->handler, &TelegramDownloadHandler::downloadedSizeChanged, this, &TelegramImageElement::downloadedSizeChanged);
    connect(p->handler, &TelegramDownloadHandler::downloadingChanged  , this, &TelegramImageElement::downloadingChanged  );
    connect(p->handler, &TelegramDownloadHandler::destinationChanged  , this, &TelegramImageElement::destinationChangeEvent);
    connect(p->handler, &TelegramDownloadHandler::thumbnailChanged    , this, &TelegramImageElement::thumbnailChangeEvent);
    connect(p->handler, &TelegramDownloadHandler::imageSizeChanged    , this, &TelegramImageElement::imageSizeChanged    );
    connect(p->handler, &TelegramDownloadHandler::thumbnailSizeChanged, this, &TelegramImageElement::imageSizeChanged    );
    connect(p->handler, &TelegramDownloadHandler::errorChanged        , this, &TelegramImageElement::errorChanged        );
}

void TelegramImageElement::setSource(TelegramTypeQObject *object)
{
    p->handler->setSource(object);
}

TelegramTypeQObject *TelegramImageElement::source() const
{
    return p->handler->source();
}

void TelegramImageElement::setEngine(TelegramEngine *engine)
{
    p->handler->setEngine(engine);
}

TelegramEngine *TelegramImageElement::engine() const
{
    return p->handler->engine();
}

void TelegramImageElement::setQtQuickVersion(const QString &version)
{
    if(p->qtQuickVersion == version)
        return;

    p->qtQuickVersion = version;
    Q_EMIT qtQuickVersionChanged();
}

const QString &TelegramImageElement::qtQuickVersion() const
{
    return p->qtQuickVersion;
}

bool TelegramImageElement::asynchronous()
{
    READ_PROPERTY(asynchronous, bool);
}

void TelegramImageElement::setAsynchronous(bool asynchronous)
{
    SET_PROPERTY(asynchronous, bool);
}

bool TelegramImageElement::autoTransform()
{
    READ_PROPERTY(autoTransform, bool);
}

void TelegramImageElement::setAutoTransform(bool autoTransform)
{
    SET_PROPERTY(autoTransform, bool);
}

bool TelegramImageElement::cache()
{
    READ_PROPERTY(cache, bool);
}

void TelegramImageElement::setCache(bool cache)
{
    SET_PROPERTY(cache, bool);
}

int TelegramImageElement::fillMode()
{
    READ_PROPERTY(fillMode, int);
}

void TelegramImageElement::setFillMode(int fillMode)
{
    SET_PROPERTY(fillMode, int);
}

int TelegramImageElement::horizontalAlignment()
{
    READ_PROPERTY(horizontalAlignment, int);
}

void TelegramImageElement::setHorizontalAlignment(int horizontalAlignment)
{
    SET_PROPERTY(horizontalAlignment, int);
    Q_EMIT horizontalAlignmentChanged();
}

bool TelegramImageElement::mipmap()
{
    READ_PROPERTY(mipmap, bool);
}

void TelegramImageElement::setMipmap(bool mipmap)
{
    SET_PROPERTY(mipmap, bool);
    Q_EMIT mipmapChanged();
}

bool TelegramImageElement::mirror()
{
    READ_PROPERTY(mirror, bool);
}

void TelegramImageElement::setMirror(bool mirror)
{
    SET_PROPERTY(mirror, bool);
}

qreal TelegramImageElement::paintedHeight()
{
    READ_PROPERTY(paintedHeight, qreal);
}

qreal TelegramImageElement::paintedWidth()
{
    READ_PROPERTY(paintedWidth, qreal);
}

bool TelegramImageElement::smooth()
{
    READ_PROPERTY(smooth, qreal);
}

void TelegramImageElement::setSmooth(bool smooth)
{
    SET_PROPERTY(smooth, qreal);
    Q_EMIT smoothChanged();
}

QSizeF TelegramImageElement::sourceSize()
{
    READ_PROPERTY(sourceSize, QSizeF);
}

void TelegramImageElement::setSourceSize(const QSizeF &sourceSize)
{
    SET_PROPERTY(sourceSize, QSizeF);
}

int TelegramImageElement::verticalAlignment()
{
    READ_PROPERTY(verticalAlignment, int);
}

void TelegramImageElement::setVerticalAlignment(int verticalAlignment)
{
    SET_PROPERTY(verticalAlignment, int);
    Q_EMIT verticalAlignmentChanged();
}

QSizeF TelegramImageElement::imageSize() const
{
    if(p->handler->imageSize().isValid())
        return p->handler->imageSize();
    else
    if(p->imageSize.isValid())
        return p->imageSize;
    else
        return p->handler->thumbnailSize();
}

qint32 TelegramImageElement::fileSize() const
{
    return p->handler->downloadTotal()? p->handler->downloadTotal() : p->handler->size();
}

qint32 TelegramImageElement::downloadedSize() const
{
    return p->handler->downloadedSize();
}

bool TelegramImageElement::downloading() const
{
    return p->handler->downloading();
}

bool TelegramImageElement::downloaded() const
{
    return !destination().isEmpty();
}

bool TelegramImageElement::thumbnailDownloaded() const
{
    return !thumbnail().isEmpty();
}

QUrl TelegramImageElement::destination() const
{
    QString path = p->handler->destination();
    if(path.isEmpty())
        return QUrl();
    else
        return QUrl::fromLocalFile(path);
}

QUrl TelegramImageElement::thumbnail() const
{
    QString path = p->handler->thumbnail();
    if(path.isEmpty())
        return QUrl();
    else
        return QUrl::fromLocalFile(path);
}

QUrl TelegramImageElement::currentImage() const
{
    if(!p->image)
        return QUrl();
    else
        return p->image->property("source").toUrl();
}

QString TelegramImageElement::errorText() const
{
    return p->handler->errorText();
}

qint32 TelegramImageElement::errorCode() const
{
    return p->handler->errorCode();
}

QStringList TelegramImageElement::requiredProperties()
{
    return QStringList() << FUNCTION_NAME(engine) << FUNCTION_NAME(source);
}

bool TelegramImageElement::download()
{
    return p->handler->download();
}

bool TelegramImageElement::check()
{
    return p->handler->check();
}

void TelegramImageElement::stop()
{
    p->handler->stop();
}

void TelegramImageElement::initImage()
{
    if(p->image)
        return;

    QQmlEngine *engine = qmlEngine(this);
    QQmlContext *context = qmlContext(this);
    if(!engine || !context)
        return;

    QQmlComponent component(engine);
    component.setData(QString("import QtQuick %1\n"
                              "Image { anchors.fill: parent; }").arg(p->qtQuickVersion).toUtf8(), QUrl());
    QQuickItem *item = qobject_cast<QQuickItem *>(component.create(context));
    if(!item)
        return;

    item->setParent(this);
    item->setParentItem(this);

    connect(item, SIGNAL(asynchronousChanged()), this, SIGNAL(asynchronousChanged()));

    #if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        connect(item, SIGNAL(autoTransformChanged()), this, SIGNAL(autoTransformChanged()));
    #endif

    connect(item, SIGNAL(cacheChanged()), this, SIGNAL(cacheChanged()));
    connect(item, SIGNAL(fillModeChanged()), this, SIGNAL(fillModeChanged()));
    connect(item, SIGNAL(mirrorChanged()), this, SIGNAL(mirrorChanged()));
    connect(item, SIGNAL(sourceSizeChanged()), this, SIGNAL(sourceSizeChanged()));

    p->image = item;
}

void TelegramImageElement::setImage(const QString &image)
{
    initImage();
    if(QFileInfo(image).exists())
    {
        QImageReader reader(image);
        p->imageSize = reader.size();
    }
    else
        p->imageSize = QSizeF();

    p->image->setProperty("source", (image.isEmpty() ? QUrl() : QUrl::fromLocalFile(image)));
    Q_EMIT imageSizeChanged();
    Q_EMIT currentImageChanged();
}

void TelegramImageElement::destinationChangeEvent()
{
    QString dest = p->handler->destination();
    if(dest.isEmpty() || !p->mdb.mimeTypeForFile(dest).name().contains("image", Qt::CaseInsensitive))
    {
        QString thumb = p->handler->thumbnail();
        setImage(thumb);
        Q_EMIT thumbnailChanged();
        Q_EMIT thumbnailDownloadedChanged();
    }
    else
        setImage(dest);

    Q_EMIT destinationChanged();
    Q_EMIT downloadedChanged();
    Q_EMIT downloadedChanged();
}

void TelegramImageElement::thumbnailChangeEvent()
{
    destinationChangeEvent();
}

TelegramImageElement::~TelegramImageElement()
{
    delete p;
}

