#include "imagesmodel.h"
#include "imagesmodelworker.h"

#include <QStandardPaths>
#include <QUrl>
#include <QThread>

#include <algorithm>


const QStringList ImagesModel::_imagesdirpaths(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation));
const QStringList ImagesModel::_filterlist(QStringList() << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp" << "*.pbm" << "*.pgm" << "*.ppm" << "*.xbm" << "*.xpm");


ImagesModel::ImagesModel(QObject *parent) : QAbstractListModel(parent),
    _worker(new ImagesModelWorker()),
    _workerthread(new QThread())
{
    qRegisterMetaType<ImagesModel::EntryList>("ImagesModel::EntryList");
    qRegisterMetaType<ImagesModel::Request>("ImagesModel::Request");

    this->_worker->moveToThread(this->_workerthread);
    this->_workerthread->start(QThread::LowPriority);

    connect(this, &ImagesModel::newRequest, this->_worker, &ImagesModelWorker::handleRequest);
    connect(this->_worker, &ImagesModelWorker::requestComplete, this, &ImagesModel::handleCompletedRequest);

    this->_request.rootDir = ImagesModel::_imagesdirpaths.first();

    emit newRequest(this->_request);
}

ImagesModel::~ImagesModel()
{
    this->_worker->deleteLater();
    this->_workerthread->quit();
    this->_workerthread->deleteLater();
}

QHash<int, QByteArray> ImagesModel::roleNames() const
{
    QHash<int, QByteArray> hash;

    hash.insert(ImagesModel::PathRole, QByteArray("path"));
    hash.insert(ImagesModel::DateRole, QByteArray("date"));
    hash.insert(ImagesModel::OrientationRole, QByteArray("orientation"));
    hash.insert(ImagesModel::UrlRole, QByteArray("url"));
    hash.insert(ImagesModel::IsDirRole, QByteArray("isDir"));
    hash.insert(ImagesModel::NameRole, QByteArray("name"));

    return hash;
}

int ImagesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return this->_entries.size();
}

QVariant ImagesModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();

    if (row >= this->_entries.size())
        return QVariant();

    switch (role)
    {
        case ImagesModel::PathRole:
        {
            return this->_entries.at(row).path;
        }
        case ImagesModel::DateRole:
        {
            return this->_entries.at(row).date;
        }
        case ImagesModel::OrientationRole:
        {
            return this->_entries.at(row).orientation;
        }
        case ImagesModel::UrlRole:
        {
            return QUrl::fromLocalFile(this->_entries.at(row).path);
        }
        case ImagesModel::IsDirRole:
        {
            return this->_entries.at(row).isDir;
        }
        case ImagesModel::NameRole:
        {
            return this->_entries.at(row).path.split('/').last();
        }
    }

    return QVariant();
}

void ImagesModel::handleCompletedRequest(const ImagesModel::Request &request, const ImagesModel::EntryList &list)
{
    if (this->_request == request)
    {
        beginResetModel();

        this->_entries = list;

        endResetModel();
    }
}

void ImagesModel::setSortOrder(Qt::SortOrder order)
{
    if (this->_request.sortOrder == order)
        return;

    this->_request.sortOrder = order;
    emit sortOrderChanged();

    emit newRequest(this->_request);
}

void ImagesModel::setSortRole(ImagesModel::Role role)
{
    if (this->_request.sortRole == role)
        return;

    this->_request.sortRole = role;
    emit sortRoleChanged();

    emit newRequest(this->_request);
}

void ImagesModel::setDirectoriesFirst(bool df)
{
    if (this->_request.directoriesFirst == df)
        return;

    this->_request.directoriesFirst = df;
    emit directoriesFirstChanged();

    emit newRequest(this->_request);
}

void ImagesModel::setRootDir(const QString &newRoot)
{
    QString temp;

    if (newRoot.isEmpty())
        temp = ImagesModel::_imagesdirpaths.first();
    else
        temp = newRoot;

    if (this->_request.rootDir == temp)
        return;

    this->_request.rootDir = temp;

    emit rootDirChanged();

    emit newRequest(this->_request);
}

void ImagesModel::setRecursive(bool recursive)
{
    if (this->_request.recursive == recursive)
        return;

    this->_request.recursive = recursive;
    emit recursiveChanged();

    emit newRequest(this->_request);
}

