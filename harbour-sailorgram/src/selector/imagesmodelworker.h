#ifndef IMAGESMODELWORKER_H
#define IMAGESMODELWORKER_H


#include <QObject>

#include "imagesmodel.h"


class ImagesModelWorker : public QObject
{
    Q_OBJECT

public:
    ImagesModelWorker(QObject *parent = Q_NULLPTR);

signals:
    void scanComplete(ImagesModel::EntryList);

public slots:
    void scanDirectory(const QString &dirPath, const QStringList &filterList);
};

#endif // IMAGESMODELWORKER_H
