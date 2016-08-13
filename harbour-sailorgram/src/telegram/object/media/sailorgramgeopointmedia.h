#ifndef SAILORGRAMGEOPOINTMEDIA_H
#define SAILORGRAMGEOPOINTMEDIA_H

#include <QObject>
#include <telegram/objects/geopointobject.h>

class SailorgramGeoPointMedia : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString address READ address CONSTANT FINAL)
    Q_PROPERTY(qreal latitude READ latitude CONSTANT FINAL)
    Q_PROPERTY(qreal longitude READ longitude CONSTANT FINAL)

    public:
        explicit SailorgramGeoPointMedia(GeoPointObject* geopoint, QString address = QString(), QObject *parent = 0);
        QString address() const;
        qreal latitude() const;
        qreal longitude() const;

    private:
        GeoPointObject* _geopoint;
        QString _address;
};

#endif // SAILORGRAMGEOPOINTMEDIA_H
