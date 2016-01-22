#include "thumbnailprovider.h"

#include <QStandardPaths>
#include <QCryptographicHash>
#include <QFile>
#include <QDir>


const QSize ThumbnailProvider::THUMBNAIL_SIZE(200, 200);

ThumbnailProvider::ThumbnailProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap, QQmlImageProviderBase::ForceAsynchronousImageLoading),
    _thumbnailsdir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/thumbs/")
{
    QDir dir(this->_thumbnailsdir);

    if (!dir.exists())
        dir.mkpath(QStringLiteral("."));
}

QPixmap ThumbnailProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    if (id.isEmpty())
        return QPixmap();

    const QByteArray hash = QCryptographicHash::hash(id.toLatin1(), QCryptographicHash::Md5);

    QString thumbnailPath(this->_thumbnailsdir + QString::fromLocal8Bit(hash.toHex()) + ".png");

    if (!QFile::exists(thumbnailPath))
    {
        QImage image(id);

        if (image.isNull())
            return QPixmap();

        const QImage scaledImage(image.scaled(THUMBNAIL_SIZE, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation));

        scaledImage.save(thumbnailPath, "PNG", 9);

        if (requestedSize.width() < scaledImage.width() || requestedSize.height() < scaledImage.height())
        {
            QPixmap pix = QPixmap::fromImage(scaledImage.scaled(requestedSize, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation), Qt::NoOpaqueDetection);
            *size = pix.size();
            return pix;
        }

        *size = scaledImage.size();
        return QPixmap::fromImage(scaledImage, Qt::NoOpaqueDetection);
    }

    QImage image(thumbnailPath);

    if (requestedSize.width() < image.width() || requestedSize.height() < image.height())
        image = image.scaled(requestedSize, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);

    *size = image.size();
    return QPixmap::fromImage(image, Qt::NoOpaqueDetection);
}
