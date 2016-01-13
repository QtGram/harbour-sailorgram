#ifndef IMAGETHUMBNAILER_H
#define IMAGETHUMBNAILER_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QQmlPropertyValueSource>
#include <QQmlProperty>

class ImageThumbnailer : public QObject, public QQmlPropertyValueSource
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_INTERFACES(QQmlPropertyValueSource)

    Q_PROPERTY(int status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString imageUrl READ imageUrl WRITE setImageUrl NOTIFY imageUrlChanged)

    public:
        enum Status { Idle, Processing, Finished, Failed };

    public:
        explicit ImageThumbnailer(QObject* parent = 0);
        ~ImageThumbnailer();
        void setTarget(const QQmlProperty& prop);
        int status() const;
        const QString& imageUrl() const;

    public:
        static QString thumbnailPathForImageUrl(const QString& imageurl);

    public slots:
        void setStatus(int status);
        void setImageUrl(const QString& imageUrl);

    signals:
        void statusChanged(const int status);
        void imageUrlChanged(const QString& imageUrl);
        void enqueueRequested(const QString& imageUrl);
        void dequeueRequested(const QString& imageUrl);

    protected slots:
        QString doProcessImage();
        void doUpdateTarget();

    private slots:
        void onProcessingDone();

    private:
        int _status;
        QString _imageurl;
        QString _thumbnailurl;
        QString _targetpropertyname;
        QObject* _targetobject;
        QFutureWatcher<QString> _watcher;
};

#endif // IMAGETHUMBNAILER_H
