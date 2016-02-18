#include "filesmodel.h"
#include "filesmodelworker.h"

#include <QStandardPaths>
#include <QUrl>
#include <QThread>
#include <QFileInfo>


QMutex FilesModel::_mutex;
FilesModelWorker *FilesModel::_worker(Q_NULLPTR);
QThread *FilesModel::_workerthread(Q_NULLPTR);
uint FilesModel::_ref(0);

void FilesModel::registerMetaTypes()
{
    qRegisterMetaType<FilesModel::EntryList>("FilesModel::EntryList");
    qRegisterMetaType<FilesModel::Request>("FilesModel::Request");
}


FilesModel::FilesModel(QObject *parent) : QAbstractListModel(parent)
{
    FilesModel::_mutex.lock();
    FilesModel::_ref++;

    if (FilesModel::_ref == 1)
    {
        FilesModel::_worker = new FilesModelWorker;
        FilesModel::_workerthread = new QThread;

        FilesModel::_worker->moveToThread(FilesModel::_workerthread);
        FilesModel::_workerthread->start();
    }

    connect(this, &FilesModel::newRequest, FilesModel::_worker, &FilesModelWorker::handleRequest);
    connect(FilesModel::_worker, &FilesModelWorker::requestComplete, this, &FilesModel::handleCompletedRequest);

    FilesModel::_mutex.unlock();
}

FilesModel::~FilesModel()
{
    FilesModel::_mutex.lock();
    FilesModel::_ref--;

    if (FilesModel::_ref == 0)
    {
        FilesModel::_worker->deleteLater();
        FilesModel::_workerthread->quit();
        FilesModel::_workerthread->deleteLater();
    }

    FilesModel::_mutex.unlock();
}

QHash<int, QByteArray> FilesModel::roleNames() const
{
    QHash<int, QByteArray> hash;

    hash.insert(FilesModel::PathRole, QByteArray("path"));
    hash.insert(FilesModel::DateRole, QByteArray("date"));
    hash.insert(FilesModel::OrientationRole, QByteArray("orientation"));
    hash.insert(FilesModel::UrlRole, QByteArray("url"));
    hash.insert(FilesModel::IsDirRole, QByteArray("isDir"));
    hash.insert(FilesModel::NameRole, QByteArray("name"));
    hash.insert(FilesModel::IconRole, QByteArray("icon"));

    return hash;
}

int FilesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return this->_entries.size();
}

QVariant FilesModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();

    if (row >= this->_entries.size())
        return QVariant();

    switch (role)
    {
        case FilesModel::PathRole:
        {
            return this->_entries.at(row).path;
        }
        case FilesModel::DateRole:
        {
            return this->_entries.at(row).date;
        }
        case FilesModel::OrientationRole:
        {
            return this->_entries.at(row).orientation;
        }
        case FilesModel::UrlRole:
        {
            return QUrl::fromLocalFile(this->_entries.at(row).path);
        }
        case FilesModel::IsDirRole:
        {
            return this->_entries.at(row).isDir;
        }
        case FilesModel::NameRole:
        {
            return this->_entries.at(row).path.split('/').last();
        }
        case FilesModel::IconRole:
        {
            if (this->_entries.at(row).isDir)
                return "image://theme/icon-m-folder";

            QString category = this->_entries.at(row).type.split('/').at(0);

            if (category == QStringLiteral("image"))
                return "image://theme/icon-m-image";

            if (category == QStringLiteral("video"))
                return "image://theme/icon-m-video";

            if (category == QStringLiteral("text"))
                return "image://theme/icon-m-document";

            if (category == QStringLiteral("audio"))
                return "image://theme/icon-m-music";

            return "image://theme/icon-m-other";
        }
    }

    return QVariant();
}

void FilesModel::handleCompletedRequest(const FilesModel::Request &request, const FilesModel::EntryList &list)
{
    if (this->_request == request)
    {
        beginResetModel();

        this->_entries = list;

        endResetModel();
    }
}

QString FilesModel::parentFolder() const
{
    return QFileInfo(this->_request.folder).canonicalPath();
}

void FilesModel::setSortOrder(Qt::SortOrder order)
{
    if (this->_request.sortOrder == order)
        return;

    this->_request.sortOrder = order;
    emit sortOrderChanged();

    emit newRequest(this->_request);
}

void FilesModel::setSortRole(FilesModel::Role role)
{
    if (this->_request.sortRole == role)
        return;

    this->_request.sortRole = role;
    emit sortRoleChanged();

    emit newRequest(this->_request);
}

void FilesModel::setFilter(FilesModel::Filter filter)
{
    if (this->_request.filter == filter)
        return;

    this->_request.filter = filter;
    emit filterChanged();

    emit newRequest(this->_request);
}

void FilesModel::setDirectoriesFirst(bool df)
{
    if (this->_request.directoriesFirst == df)
        return;

    this->_request.directoriesFirst = df;
    emit directoriesFirstChanged();

    emit newRequest(this->_request);
}

void FilesModel::setFolder(const QString &folder)
{
    QString temp;

    if (folder.isEmpty() || folder == QStringLiteral("HomeFolder"))
        temp = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).value(0);
    else if (folder == QStringLiteral("StandardImagesFolder"))
        temp = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).value(0);
    else if (folder == QStringLiteral("StandardVideosFolder"))
        temp = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0);
    else if (folder == QStringLiteral("StandardDocumentsFolder"))
        temp = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).value(0);
    else if (folder == QStringLiteral("StandardMusicFolder"))
        temp = QStandardPaths::standardLocations(QStandardPaths::MusicLocation).value(0);
    else
        temp = folder;

    if (temp.endsWith('/') && temp.size() > 1)
        temp.chop(1);

    if (this->_request.folder == temp || !temp.startsWith('/'))
        return;

    this->_request.folder = temp;

    emit folderChanged();

    emit newRequest(this->_request);
}

void FilesModel::setRecursive(bool recursive)
{
    if (this->_request.recursive == recursive)
        return;

    this->_request.recursive = recursive;
    emit recursiveChanged();

    emit newRequest(this->_request);
}

