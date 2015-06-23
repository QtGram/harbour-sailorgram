/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *      Roberto Mier
 *      Tiago Herrmann
 */

#ifndef GEOPOINT_H
#define GEOPOINT_H
class GeoPoint
{
public:

    enum GeoPointType {
       typeGeoPoint = 0x2049d70c,
       typeGeoPointEmpty = 0x1117dd5f
    };

    GeoPoint(GeoPointType classType) :
        m_longitude(0.0),
        m_lat(0.0),
        m_classType(classType) {}

    void setLongitude(double longitude) {
        m_longitude = longitude;
    }
    double longitude() const {
        return m_longitude;
    }
    void setLat(double lat) {
        m_lat = lat;
    }
    double lat() const {
        return m_lat;
    }
    void setClassType(GeoPointType classType) {
        m_classType = classType;
    }
    GeoPointType classType() const {
        return m_classType;
    }

private:
    double m_longitude;
    double m_lat;
    GeoPointType m_classType;
};
#endif // GEOPOINT_H
