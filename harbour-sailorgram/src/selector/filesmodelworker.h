#ifndef FILESMODELWORKER_H
#define FILESMODELWORKER_H


#include <QObject>
#include <QStringList>

#include "filesmodel.h"


class FilesModelWorker : public QObject
{
    Q_OBJECT

public:

    FilesModelWorker() {}
    ~FilesModelWorker() {}

public slots:

    void handleRequest(const FilesModel::Request &request);

signals:

    void requestComplete(FilesModel::Request, FilesModel::EntryList);

private:

    static bool lesserPathThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2);
    static bool lesserDateThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2);
    static bool lesserNameThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2);
    static bool lesserOrientationThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2);
    static bool lesserIsDirThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2);
    static bool lesserTypeThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2);
    static bool biggerPathThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2);
    static bool biggerDateThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2);
    static bool biggerNameThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2);
    static bool biggerOrientationThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2);
    static bool biggerIsDirThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2);
    static bool biggerTypeThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2);

    void sort(FilesModel::EntryList *list, Qt::SortOrder sortOrder, FilesModel::Role sortRole, bool directoriesFirst);
    FilesModel::EntryList scanDirectory(const QString &path, FilesModel::Filter filter, bool reqursive);

    FilesModel::EntryList _lastlist;
    FilesModel::Request _lastrequest;
};

#endif // FILESMODELWORKER_H
