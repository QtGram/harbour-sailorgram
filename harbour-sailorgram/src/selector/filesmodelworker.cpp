#include "filesmodelworker.h"
#include "exif/exif.h"

#include <QFileInfoList>
#include <QDir>
#include <QDateTime>
#include <QMimeDatabase>
#include <QMimeType>


void FilesModelWorker::handleRequest(const FilesModel::Request &request)
{
    if (request == this->_lastrequest)
    {
        emit requestComplete(this->_lastrequest, this->_lastlist);
        return;
    }

    FilesModel::EntryList entryList;

    if (request.folder != this->_lastrequest.folder ||
        request.recursive != this->_lastrequest.recursive ||
        request.filter != this->_lastrequest.filter)
        entryList = scanDirectory(request.folder, request.filter, request.recursive);
    else
        entryList = this->_lastlist;

    sort(&entryList, request.sortOrder, request.sortRole, request.directoriesFirst);

    this->_lastlist = entryList;
    this->_lastrequest = request;

    emit requestComplete(request, entryList);
}

FilesModel::EntryList FilesModelWorker::scanDirectory(const QString &path, FilesModel::Filter filter, bool recursive)
{
    FilesModel::EntryList entryList;

    QDir directory(path);

    if (directory.exists())
    {
        QFileInfoList list = directory.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files | QDir::Readable);
        QMimeDatabase mimedb;

        entryList.reserve(list.size());

        foreach (const QFileInfo &fileInfo, list)
        {
            const QString filePath(fileInfo.absoluteFilePath());

            FilesModel::Entry entry;
            entry.path = filePath;
            entry.path.squeeze();
            entry.date = fileInfo.lastModified().toMSecsSinceEpoch();

            if (fileInfo.isDir())
            {
                if (recursive)
                    entryList << scanDirectory(filePath, filter, recursive);
                else
                {
                    entry.isDir = true;
                    entry.orientation = 0;
                    entryList << entry;
                }
            }

            if (fileInfo.isFile())
            {
                entry.type = mimedb.mimeTypeForFile(fileInfo, QMimeDatabase::MatchExtension).name();

                QString category = entry.type.split('/').at(0);

                switch(filter)
                {
                    case FilesModel::NoFilter:
                        break;

                    case FilesModel::ImageFilter:
                    {
                        if (category != QStringLiteral("image"))
                            continue;

                        break;
                    }

                    case FilesModel::DocumentFilter:
                    {
                        if (category != QStringLiteral("text"))
                            continue;

                        break;
                    }

                    case FilesModel::VideoFilter:
                    {
                        if (category != QStringLiteral("video"))
                            continue;

                        break;
                    }

                    case FilesModel::AudioFilter:
                    {
                        if (category != QStringLiteral("audio"))
                            continue;

                        break;
                    }

                    default:
                        break;
                }

                entry.isDir = false;

                int orientation = 0;
                Exif::getExifOrientation(filePath, &orientation);

                int decodedOrientation = 0;

                switch (orientation)
                {
                    case 1:
                        decodedOrientation = 0;
                        break;

                    case 3:
                        decodedOrientation = 180;
                        break;

                    case 6:
                        decodedOrientation = 270;
                        break;

                    case 8:
                        decodedOrientation = 90;
                        break;

                    default:
                        decodedOrientation = 0;
                        break;
                }

                entry.orientation = decodedOrientation;
                entryList << entry;
            }
        }
    }

    return entryList;
}

void FilesModelWorker::sort(FilesModel::EntryList *list, Qt::SortOrder sortOrder, FilesModel::Role sortRole, bool directoriesFirst)
{
    switch (sortOrder)
    {
        case Qt::AscendingOrder:
        {
            switch(sortRole)
            {
                case FilesModel::PathRole:
                    std::sort(list->begin(), list->end(), lesserPathThan);
                    break;

                case FilesModel::UrlRole:
                    std::sort(list->begin(), list->end(), lesserPathThan);
                    break;

                case FilesModel::NameRole:
                    std::sort(list->begin(), list->end(), lesserNameThan);
                    break;

                case FilesModel::DateRole:
                    std::sort(list->begin(), list->end(), lesserDateThan);
                    break;

                case FilesModel::OrientationRole:
                    std::sort(list->begin(), list->end(), lesserOrientationThan);
                    break;

                case FilesModel::IsDirRole:
                    std::sort(list->begin(), list->end(), lesserIsDirThan);
                    break;

                case FilesModel::IconRole:
                    std::sort(list->begin(), list->end(), lesserTypeThan);
                    break;
            }
            break;
        }

        case Qt::DescendingOrder:
        {
            switch(sortRole)
            {
                case FilesModel::PathRole:
                    std::sort(list->begin(), list->end(), biggerPathThan);
                    break;

                case FilesModel::UrlRole:
                    std::sort(list->begin(), list->end(), biggerPathThan);
                    break;

                case FilesModel::NameRole:
                    std::sort(list->begin(), list->end(), biggerNameThan);
                    break;

                case FilesModel::DateRole:
                    std::sort(list->begin(), list->end(), biggerDateThan);
                    break;

                case FilesModel::OrientationRole:
                    std::sort(list->begin(), list->end(), biggerOrientationThan);
                    break;

                case FilesModel::IsDirRole:
                    std::sort(list->begin(), list->end(), biggerIsDirThan);
                    break;

                case FilesModel::IconRole:
                    std::sort(list->begin(), list->end(), biggerTypeThan);
                    break;
            }
            break;
        }
    }

    if (directoriesFirst)
        std::stable_sort(list->begin(), list->end(), biggerIsDirThan);
}

bool FilesModelWorker::lesserPathThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2)
{
    return e1.path.compare(e2.path) < 0;
}

bool FilesModelWorker::lesserNameThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2)
{
    return e1.path.split('/').last().compare(e2.path.split('/').last()) < 0;
}

bool FilesModelWorker::lesserDateThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2)
{
    return e1.date < e2.date;
}

bool FilesModelWorker::lesserOrientationThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2)
{
    return e1.orientation < e2.orientation;
}

bool FilesModelWorker::lesserIsDirThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2)
{
    return !e1.isDir && e2.isDir;
}

bool FilesModelWorker::lesserTypeThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2)
{
    return e1.type < e2.type;
}

bool FilesModelWorker::biggerPathThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2)
{
    return e1.path.compare(e2.path) > 0;
}

bool FilesModelWorker::biggerNameThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2)
{
    return e1.path.split('/').last().compare(e2.path.split('/').last()) > 0;
}

bool FilesModelWorker::biggerDateThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2)
{
    return e1.date > e2.date;
}

bool FilesModelWorker::biggerOrientationThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2)
{
    return e1.orientation > e2.orientation;
}

bool FilesModelWorker::biggerIsDirThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2)
{
    return e1.isDir && !e2.isDir;
}

bool FilesModelWorker::biggerTypeThan(const FilesModel::Entry &e1, const FilesModel::Entry &e2)
{
    return e1.type > e2.type;
}
