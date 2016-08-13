#ifndef SAILORGRAMWEBPAGEMEDIA_H
#define SAILORGRAMWEBPAGEMEDIA_H

#include <QObject>
#include <telegram/objects/webpageobject.h>

class SailorgramWebPageMedia : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString url READ url CONSTANT FINAL)
    Q_PROPERTY(QString title READ title CONSTANT FINAL)
    Q_PROPERTY(QString description READ description CONSTANT FINAL)
    Q_PROPERTY(bool hasThumbnail READ hasThumbnail CONSTANT FINAL)
    Q_PROPERTY(bool hasPureImage READ hasPureImage CONSTANT FINAL)

    public:
        explicit SailorgramWebPageMedia(WebPageObject* webpage, QObject *parent = 0);
        QString url() const;
        QString title() const;
        QString description() const;
        bool hasThumbnail() const;
        bool hasPureImage() const;

    private:
        WebPageObject* _webpage;
};

#endif // SAILORGRAMWEBPAGEMEDIA_H
