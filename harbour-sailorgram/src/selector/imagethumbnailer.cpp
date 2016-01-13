#include "imagethumbnailer.h"
#include <QGuiApplication>
#include <QImage>
#include <QUrl>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QStringBuilder>
#include <QFile>
#include <QDebug>

ImageThumbnailer::ImageThumbnailer(QObject * parent) : QObject(parent), _status(Idle), _targetobject(NULL)
{
    connect(&this->_watcher, &QFutureWatcher<QString>::finished, this, &ImageThumbnailer::onProcessingDone);
}

ImageThumbnailer::~ImageThumbnailer()
{
    this->_imageurl.clear();
}

QString ImageThumbnailer::thumbnailPathForImageUrl(const QString & imageurl)
{
    static const QString THUMBNAILS_DIR = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    const QByteArray hash = QCryptographicHash::hash(imageurl.toLatin1(), QCryptographicHash::Md5);

    return(THUMBNAILS_DIR % "/thumb-" % QString::fromLocal8Bit(hash.toHex()) % ".png");
}

int ImageThumbnailer::status() const
{
    return this->_status;
}

const QString & ImageThumbnailer::imageUrl() const
{
    return this->_imageurl;
}

void ImageThumbnailer::setStatus(int status)
{
    if(this->_status == status)
        return;

    this->_status = status;
    emit statusChanged(status);
}

void ImageThumbnailer::setImageUrl(const QString& imageUrl)
{
    if(this->_imageurl == imageUrl)
        return;

    this->_imageurl = imageUrl;

    if(this->_imageurl.isEmpty())
    {
        emit imageUrlChanged(imageUrl);
        return;
    }

    const QString tmp = ImageThumbnailer::thumbnailPathForImageUrl(this->_imageurl);

    if(QFile::exists(tmp))
    {
        this->_thumbnailurl = tmp;
        this->doUpdateTarget();
    }
    else
    {
        this->setStatus(Processing);
        this->_watcher.setFuture(QtConcurrent::run(this, &ImageThumbnailer::doProcessImage));
    }

    emit imageUrlChanged(imageUrl);
}

QString ImageThumbnailer::doProcessImage()
{
    if(this->_imageurl.isEmpty())
        return QString();

    static const QSize THUMBNAIL_SIZE(150, 150);
    QString imageurl = _imageurl;
    QImage image(QUrl(imageurl).toLocalFile());

    if(image.isNull())
        return QString();

    QImage tmp = image.scaled(THUMBNAIL_SIZE, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);
    QString ret = ImageThumbnailer::thumbnailPathForImageUrl(imageurl);

    if(!tmp.save(ret, "PNG", 9))
        ret.clear();

    return ret;
}

void ImageThumbnailer::doUpdateTarget()
{
    if(this->_targetobject == NULL || this->_targetpropertyname.isEmpty() || this->_thumbnailurl.isEmpty())
        return;

    this->_targetobject->setProperty(this->_targetpropertyname.toLatin1(), QVariant::fromValue(QUrl::fromLocalFile(this->_thumbnailurl)));
}

void ImageThumbnailer::onProcessingDone()
{
    this->_thumbnailurl = this->_watcher.result();

    if(this->_thumbnailurl.isEmpty())
    {
        this->setStatus(Failed);
        return;
    }

    this->setStatus(Finished);
    this->doUpdateTarget();
}

void ImageThumbnailer::setTarget(const QQmlProperty& prop)
{
    this->_targetobject = prop.object();
    this->_targetpropertyname = prop.name();
    this->doUpdateTarget();
}
