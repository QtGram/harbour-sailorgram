/*
    Copyright (C) 2015 Canonical

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QByteArray>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QString>
#include <QMetaMethod>
#include "telegramthumbnailer.h"

TelegramThumbnailer::TelegramThumbnailer(QObject *parent) : QObject(parent)
{
    core = new TelegramThumbnailerCore();
    thread = new QThread(this);
    thread->start();

    core->moveToThread(thread);

    connect(core, SIGNAL(thumbnailCreated(QString)), SLOT(thumbnailCreated(QString)), Qt::QueuedConnection);
}

TelegramThumbnailer::~TelegramThumbnailer()
{
    thread->quit();
    thread->wait();

    thread->deleteLater();
    thread = 0;
    
    core->deleteLater();
    core = 0;
}

QString TelegramThumbnailer::getThumbFilename(const QString &filePath) const {
    QByteArray bytes(filePath.toStdString().c_str());
    QString filename = QString(QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex());
    return filename.append(".jpg");
}

QString TelegramThumbnailer::getThumbPath(const QString &thumbnailDirPath, const QString &filePath) const {
    QString filename = getThumbFilename(filePath);
    QDir temp(thumbnailDirPath);
    QString thumbPath = temp.filePath(filename);
    return thumbPath;
}

bool TelegramThumbnailer::hasThumbnail(const QString &thumbnailDirPath, const QString &filePath) const {
    QString thumbPath = getThumbPath(thumbnailDirPath, filePath);
    return hasThumbnail(thumbPath);
}

bool TelegramThumbnailer::hasThumbnail(const QString &thumbPath) const {
    QFile thumbnail(thumbPath);
    return thumbnail.exists();
}

void TelegramThumbnailer::createThumbnail(const QString &source, const QString &dest, TelegramThumbnailer_Callback callback) {
    qDebug() << "thumbnailer: creating thumbnail";
    requests.insert(source, callback);

    QMetaObject::invokeMethod(core, "createThumbnail", Qt::QueuedConnection,
            Q_ARG(QString,source), Q_ARG(QString,dest));
}

void TelegramThumbnailer::thumbnailCreated(QString path) {
    qDebug() << "thumbnailer: finished";
    TelegramThumbnailer_Callback callback = requests.take(path);
#ifdef TG_THUMBNAILER_CPP11
    if (callback) {
        callback();
    }
#else
    if(callback.object)
        call(callback.object, callback.method, Qt::DirectConnection, callback.args);
#endif
}

QVariant TelegramThumbnailer::call(QObject *obj, const QString &member, Qt::ConnectionType ctype, QVariantList vals)
{
    /*! This method taken from AsemanQtTools -> AsemanTools class !*/
    const QMetaObject *meta_obj = obj->metaObject();
    QMetaMethod meta_method;
    for( int i=0; i<meta_obj->methodCount(); i++ )
    {
        QMetaMethod mtd = meta_obj->method(i);
        if( mtd.name() == member )
            meta_method = mtd;
    }
    if( !meta_method.isValid() )
        return QVariant();

    QList<QByteArray> param_types = meta_method.parameterTypes();
    QList<QByteArray> param_names = meta_method.parameterNames();

    QString ret_type = meta_method.typeName();
    QList< QPair<QString,QString> > m_args;
    for( int i=0 ; i<param_types.count() ; i++ )
        m_args << QPair<QString,QString>( param_types.at(i) , param_names.at(i) );

    for(int i=vals.count(); i<10; i++)
        vals << QVariant();

    QVariantList tr_vals;

    QList< QPair<QString,const void*> > args;
    for( int i=0 ; i<vals.count() ; i++ )
    {
        if( i<m_args.count() )
        {
            QString type = m_args.at(i).first;

            if( type != vals.at(i).typeName() )
            {
                if( !vals[i].canConvert( QVariant::nameToType(type.toLatin1()) ) )
                    ;
                else
                    vals[i].convert( QVariant::nameToType(type.toLatin1()) );
            }

            args << QPair<QString,const void*>( type.toLatin1() , vals.at(i).data() );
            tr_vals << vals[i];
        }
        else
        {
            args << QPair<QString,const void*>( vals.at(i).typeName() , vals.at(i).data() );
        }
    }

    int type = QMetaType::type(ret_type.toLatin1());
    void *res = QMetaType::create( type );
    bool is_pointer = ret_type.contains('*');

    bool done;
    switch( static_cast<int>(ctype) )
    {
    case Qt::QueuedConnection:
        done = QMetaObject::invokeMethod( obj , member.toLatin1() , Qt::QueuedConnection ,
                                  QGenericArgument( args.at(0).first.toLatin1() , args.at(0).second ) ,
                                  QGenericArgument( args.at(1).first.toLatin1() , args.at(1).second ) ,
                                  QGenericArgument( args.at(2).first.toLatin1() , args.at(2).second ) ,
                                  QGenericArgument( args.at(3).first.toLatin1() , args.at(3).second ) ,
                                  QGenericArgument( args.at(4).first.toLatin1() , args.at(4).second ) ,
                                  QGenericArgument( args.at(5).first.toLatin1() , args.at(5).second ) ,
                                  QGenericArgument( args.at(6).first.toLatin1() , args.at(6).second ) ,
                                  QGenericArgument( args.at(7).first.toLatin1() , args.at(7).second ) ,
                                  QGenericArgument( args.at(8).first.toLatin1() , args.at(8).second ) ,
                                  QGenericArgument( args.at(9).first.toLatin1() , args.at(9).second ) );
        return QVariant();
        break;

    default:
        done = QMetaObject::invokeMethod( obj , member.toLatin1() , ctype, QGenericReturnArgument( ret_type.toLatin1() , (is_pointer)? &res : res ) ,
                                  QGenericArgument( args.at(0).first.toLatin1() , args.at(0).second ) ,
                                  QGenericArgument( args.at(1).first.toLatin1() , args.at(1).second ) ,
                                  QGenericArgument( args.at(2).first.toLatin1() , args.at(2).second ) ,
                                  QGenericArgument( args.at(3).first.toLatin1() , args.at(3).second ) ,
                                  QGenericArgument( args.at(4).first.toLatin1() , args.at(4).second ) ,
                                  QGenericArgument( args.at(5).first.toLatin1() , args.at(5).second ) ,
                                  QGenericArgument( args.at(6).first.toLatin1() , args.at(6).second ) ,
                                  QGenericArgument( args.at(7).first.toLatin1() , args.at(7).second ) ,
                                  QGenericArgument( args.at(8).first.toLatin1() , args.at(8).second ) ,
                                  QGenericArgument( args.at(9).first.toLatin1() , args.at(9).second ) );
        break;
    }

    QVariant result;
    if( !done )
        return result;

    if( type == QMetaType::Void )
        result = QVariant();
    else
    if( is_pointer )
        result = QVariant( type , &res );
    else
        result = QVariant( type , res );

    if( type == QMetaType::type("QVariant") )
        return result.value<QVariant>();
    else
        return result;
}
