#include "sailorgramgeopointmedia.h"

SailorgramGeoPointMedia::SailorgramGeoPointMedia(GeoPointObject* geopoint, QString address, QObject *parent) : QObject(parent), _geopoint(geopoint), _address(address)
{

}

QString SailorgramGeoPointMedia::address() const
{
    return this->_address;
}

qreal SailorgramGeoPointMedia::latitude() const
{
    return this->_geopoint->lat();
}

qreal SailorgramGeoPointMedia::longitude() const
{
    return this->_geopoint->longValue();
}

