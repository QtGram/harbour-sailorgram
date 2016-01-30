#include "imagesmodelworker.h"
#include "exif/exif.h"

#include <QFileInfoList>
#include <QDir>
#include <QDateTime>


ImagesModelWorker::ImagesModelWorker(QObject *parent) : QObject(parent)
{
}

void ImagesModelWorker::handleRequest(const ImagesModel::Request &request)
{
    if (request == this->_lastrequest)
    {
        emit requestComplete(this->_lastrequest, this->_lastlist);
        return;
    }

    ImagesModel::EntryList entryList;

    if (request.rootDir != this->_lastrequest.rootDir || request.recursive != this->_lastrequest.recursive)
        entryList = scanDirectory(request.rootDir, ImagesModel::_filterlist, request.recursive);
    else
        entryList = this->_lastlist;

    sort(&entryList, request.sortOrder, request.sortRole, request.directoriesFirst);

    this->_lastlist = entryList;
    this->_lastrequest = request;

    emit requestComplete(request, entryList);
}

ImagesModel::EntryList ImagesModelWorker::scanDirectory(const QString &path, const QStringList &filterList, bool recursive)
{
    ImagesModel::EntryList entryList;

    QDir directory(path);

    if (directory.exists())
    {
        QFileInfoList list = directory.entryInfoList(filterList, QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files | QDir::Readable);

        entryList.reserve(list.size());

        foreach (const QFileInfo &fileInfo, list)
        {
            const QString filePath(fileInfo.absoluteFilePath());

            ImagesModel::Entry entry;
            entry.path = filePath;
            entry.path.squeeze();
            entry.date = fileInfo.lastModified().toMSecsSinceEpoch();

            if (fileInfo.isDir())
            {
                if (recursive)
                    entryList << scanDirectory(filePath, filterList, recursive);
                else
                {
                    entry.isDir = true;
                    entry.orientation = 0;
                    entryList << entry;
                }
            }

            if (fileInfo.isFile())
            {
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

void ImagesModelWorker::sort(ImagesModel::EntryList *list, Qt::SortOrder sortOrder, ImagesModel::Role sortRole, bool directoriesFirst)
{
    switch (sortOrder)
    {
        case Qt::AscendingOrder:
        {
            switch(sortRole)
            {
                case ImagesModel::PathRole:
                    std::sort(list->begin(), list->end(), lesserPathThan);
                    break;

                case ImagesModel::UrlRole:
                    std::sort(list->begin(), list->end(), lesserPathThan);
                    break;

                case ImagesModel::NameRole:
                    std::sort(list->begin(), list->end(), lesserNameThan);
                    break;

                case ImagesModel::DateRole:
                    std::sort(list->begin(), list->end(), lesserDateThan);
                    break;

                case ImagesModel::OrientationRole:
                    std::sort(list->begin(), list->end(), lesserOrientationThan);
                    break;

                case ImagesModel::IsDirRole:
                    std::sort(list->begin(), list->end(), lesserIsDirThan);
                    break;
            }
            break;
        }

        case Qt::DescendingOrder:
        {
            switch(sortRole)
            {
                case ImagesModel::PathRole:
                    std::sort(list->begin(), list->end(), biggerPathThan);
                    break;

                case ImagesModel::UrlRole:
                    std::sort(list->begin(), list->end(), biggerPathThan);
                    break;

                case ImagesModel::NameRole:
                    std::sort(list->begin(), list->end(), biggerNameThan);
                    break;

                case ImagesModel::DateRole:
                    std::sort(list->begin(), list->end(), biggerDateThan);
                    break;

                case ImagesModel::OrientationRole:
                    std::sort(list->begin(), list->end(), biggerOrientationThan);
                    break;

                case ImagesModel::IsDirRole:
                    std::sort(list->begin(), list->end(), biggerIsDirThan);
                    break;
            }
            break;
        }
    }

    if (directoriesFirst)
        std::stable_sort(list->begin(), list->end(), biggerIsDirThan);
}

bool ImagesModelWorker::lesserPathThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2)
{
    return e1.path.compare(e2.path) < 0;
}

bool ImagesModelWorker::lesserNameThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2)
{
    return e1.path.split('/').last().compare(e2.path.split('/').last()) < 0;
}

bool ImagesModelWorker::lesserDateThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2)
{
    return e1.date < e2.date;
}

bool ImagesModelWorker::lesserOrientationThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2)
{
    return e1.orientation < e2.orientation;
}

bool ImagesModelWorker::lesserIsDirThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2)
{
    return !e1.isDir && e2.isDir;
}

bool ImagesModelWorker::biggerPathThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2)
{
    return e1.path.compare(e2.path) > 0;
}

bool ImagesModelWorker::biggerNameThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2)
{
    return e1.path.split('/').last().compare(e2.path.split('/').last()) > 0;
}

bool ImagesModelWorker::biggerDateThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2)
{
    return e1.date > e2.date;
}

bool ImagesModelWorker::biggerOrientationThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2)
{
    return e1.orientation > e2.orientation;
}

bool ImagesModelWorker::biggerIsDirThan(const ImagesModel::Entry &e1, const ImagesModel::Entry &e2)
{
    return e1.isDir && !e2.isDir;
}
