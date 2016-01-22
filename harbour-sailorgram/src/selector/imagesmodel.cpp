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
    _workerthread(new QThread()),
    _sortrole(ImagesModel::DateRole),
    _sortorder(Qt::DescendingOrder),
    _directoriesfirst(true),
    _recursive(false),
    _rootdir(ImagesModel::_imagesdirpaths.first())
{
    qRegisterMetaType<ImagesModel::EntryList>("ImagesModel::EntryList");

    this->_worker->moveToThread(this->_workerthread);
    this->_workerthread->start(QThread::LowPriority);

    connect(this, &ImagesModel::dirNeedsScan, this->_worker, &ImagesModelWorker::scanDirectory);
    connect(this->_worker, &ImagesModelWorker::scanComplete, this, &ImagesModel::integrateDirectory);

    emit dirNeedsScan(this->_rootdir, ImagesModel::_filterlist, this->_recursive);
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

void ImagesModel::sort(bool emitReset)
{
    if (emitReset)
        beginResetModel();

    switch (this->_sortorder)
    {
        case Qt::AscendingOrder:
        {
            std::sort(this->_entries.begin(), this->_entries.end(), [this] (const Entry &e1, const Entry &e2) -> bool { return lesserThan(e1, e2, this->_sortrole); });
            break;
        }

        case Qt::DescendingOrder:
        {
            std::sort(this->_entries.begin(), this->_entries.end(), [this] (const Entry &e1, const Entry &e2) -> bool { return biggerThan(e1, e2, this->_sortrole); });
            break;
        }
    }

    if (this->_directoriesfirst)
        std::stable_sort(this->_entries.begin(), this->_entries.end(), [this] (const Entry &e1, const Entry &e2) -> bool { return biggerThan(e1, e2, ImagesModel::IsDirRole); });

    if (emitReset)
        endResetModel();
}

void ImagesModel::integrateDirectory(const ImagesModel::EntryList &list, const QString &dirPath)
{
    if (this->_recursive && dirPath.contains(this->_rootdir))
    {
        beginResetModel();

        this->_entries += list;
        sort(false);

        endResetModel();
    }

    if (!this->_recursive && dirPath == this->_rootdir)
    {
        beginResetModel();

        this->_entries = list;
        sort(false);

        endResetModel();
    }
}

void ImagesModel::setSortOrder(Qt::SortOrder order)
{
    if (this->_sortorder == order)
        return;

    this->_sortorder = order;
    emit sortOrderChanged();

    sort();
}

void ImagesModel::setSortRole(ImagesModel::Role role)
{
    if (this->_sortrole == role)
        return;

    this->_sortrole = role;
    emit sortRoleChanged();

    sort();
}

void ImagesModel::setDirectoriesFirst(bool df)
{
    if (this->_directoriesfirst == df)
        return;

    this->_directoriesfirst = df;
    emit directoriesFirstChanged();

    sort();
}

void ImagesModel::setRootDir(const QString &newRoot)
{
    QString temp;

    if (newRoot.isEmpty())
        temp = ImagesModel::_imagesdirpaths.first();
    else
        temp = newRoot;

    if (this->_rootdir == temp)
        return;

    this->_rootdir = temp;

    emit rootDirChanged();

    beginResetModel();
    this->_entries.clear();
    endResetModel();

    emit dirNeedsScan(this->_rootdir, ImagesModel::_filterlist, this->_recursive);
}

void ImagesModel::setRecursive(bool recursive)
{
    if (this->_recursive == recursive)
        return;

    this->_recursive = recursive;
    emit recursiveChanged();

    if (this->_recursive)
    {
        foreach (const ImagesModel::Entry &entry, this->_entries)
        {
            if (entry.isDir)
                emit dirNeedsScan(entry.path, ImagesModel::_filterlist, this->_recursive);
        }
    }
    else
    {
        int rootLength = this->_rootdir.split('/').length();

        for (int i=0; i<this->_entries.size(); i++)
        {
            if (this->_entries.at(i).path.split('/').length() > rootLength)
            {
                beginRemoveRows(QModelIndex(), i, i);
                this->_entries.removeAt(i);
                endRemoveRows();
                i--;
            }
        }
    }
}

bool ImagesModel::lesserThan(const Entry &e1, const Entry &e2, ImagesModel::Role role)
{
    switch (role)
    {
        case ImagesModel::PathRole:
            return e1.path.compare(e2.path) < 0;

        case ImagesModel::DateRole:
            return e1.date < e2.date;

        case ImagesModel::OrientationRole:
            return e1.orientation < e2.orientation;

        case ImagesModel::UrlRole:
            return e1.path < e2.path;

        case ImagesModel::IsDirRole:
            return e1.isDir < e2.isDir;

        case ImagesModel::NameRole:
            return e1.path.split('/').last() < e2.path.split('/').last();
    }

    return false;
}

bool ImagesModel::biggerThan(const Entry &e1, const Entry &e2, ImagesModel::Role role)
{
    switch (role)
    {
        case ImagesModel::PathRole:
            return e1.path.compare(e2.path) > 0;

        case ImagesModel::DateRole:
            return e1.date > e2.date;

        case ImagesModel::OrientationRole:
            return e1.orientation > e2.orientation;

        case ImagesModel::UrlRole:
            return e1.path > e2.path;

        case ImagesModel::IsDirRole:
            return e1.isDir > e2.isDir;

        case ImagesModel::NameRole:
            return e1.path.split('/').last() > e2.path.split('/').last();
    }

    return false;
}

#include "moc_imagesmodel.cpp"
