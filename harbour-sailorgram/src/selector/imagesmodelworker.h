#ifndef IMAGESMODELWORKER_H
#define IMAGESMODELWORKER_H


#include <QObject>

#include "imagesmodel.h"


class ImagesModelWorker : public QObject
{
    Q_OBJECT

public:

    ImagesModelWorker(QObject *parent = Q_NULLPTR);

public slots:

    void handleRequest(const ImagesModel::Request &request);

signals:

    void requestComplete(ImagesModel::Request, ImagesModel::EntryList);

private:

    static bool lesserPathThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2);
    static bool lesserDateThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2);
    static bool lesserNameThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2);
    static bool lesserOrientationThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2);
    static bool lesserIsDirThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2);
    static bool biggerPathThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2);
    static bool biggerDateThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2);
    static bool biggerNameThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2);
    static bool biggerOrientationThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2);
    static bool biggerIsDirThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2);

    void sort(ImagesModel::EntryList *list, Qt::SortOrder sortOrder, ImagesModel::Role sortRole, bool directoriesFirst);
    ImagesModel::EntryList scanDirectory(const QString &path, const QStringList &filterList, bool reqursive);

    ImagesModel::EntryList _lastlist;
    ImagesModel::Request _lastrequest;
};

#endif // IMAGESMODELWORKER_H
