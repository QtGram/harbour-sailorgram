#ifndef THUMBNAILPROVIDER_H
#define THUMBNAILPROVIDER_H


#include <QQuickImageProvider>


class ThumbnailProvider : public QQuickImageProvider
{
public:
    ThumbnailProvider();

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) Q_DECL_OVERRIDE;

private:
    static const QSize THUMBNAIL_SIZE;

    const QString _thumbnailsdir;
};

#endif // THUMBNAILPROVIDER_H
