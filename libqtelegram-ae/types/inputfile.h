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

#ifndef INPUTFILE_H
#define INPUTFILE_H

#include <QString>

class InputFile
{
public:

    enum InputFileType {
       typeInputFileBig = 0xfa4f0bb5,
       typeInputFile = 0xf52ff27f
    };

    InputFile(InputFileType classType) :
        m_md5Checksum(""),
        m_parts(0),
        m_id(0),
        m_name(""),
        m_classType(classType) {}

    void setId(qint64 id) {
        m_id = id;
    }
    qint64 id() const {
        return m_id;
    }
    void setParts(qint32 parts) {
        m_parts = parts;
    }
    qint32 parts() const {
        return m_parts;
    }
    void setName(const QString & name) {
        m_name = name;
    }
    QString name() const {
        return m_name;
    }
    void setMd5Checksum(const QString & md5Checksum) {
        m_md5Checksum = md5Checksum;
    }
    QString md5Checksum() const {
        return m_md5Checksum;
    }
    void setClassType(InputFileType classType) {
        m_classType = classType;
    }
    InputFileType classType() const {
        return m_classType;
    }

private:
    QString m_md5Checksum;
    qint32 m_parts;
    qint64 m_id;
    QString m_name;
    InputFileType m_classType;
};
#endif // INPUTFILE_H
