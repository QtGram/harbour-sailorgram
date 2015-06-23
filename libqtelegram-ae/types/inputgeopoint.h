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

#ifndef INPUTGEOPOINT_H
#define INPUTGEOPOINT_H
class InputGeoPoint
{
public:

    enum InputGeoPointType {
       typeInputGeoPointEmpty = 0xe4c123d6,
       typeInputGeoPoint = 0xf3b7acc9
    };

    InputGeoPoint(InputGeoPointType classType =  typeInputGeoPointEmpty) :
        m_longitude(0.0),
        m_lat(0.0),
        m_classType(classType) {}

    void setLat(double lat) { m_lat = lat; }
    double lat() const { return m_lat; }
    void setLongitude(double longitude) { m_longitude = longitude; }
    double longitude() const { return m_longitude; }
    void setClassType(InputGeoPointType classType) { m_classType = classType; }
    InputGeoPointType classType() const { return m_classType;}

private:
    double m_longitude;
    double m_lat;
    InputGeoPointType m_classType;
};
#endif // INPUTGEOPOINT_H
