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
    _workerthread(new QThread(this)),
    _sortrole(ImagesModel::DateRole),
    _sortorder(Qt::DescendingOrder)
{
    qRegisterMetaType<ImagesModel::EntryList>("ImagesModel::EntryList");

    this->_worker->moveToThread(this->_workerthread);
    this->_workerthread->start(QThread::LowPriority);

    connect(this, &ImagesModel::dirNeedsScan, this->_worker, &ImagesModelWorker::scanDirectory);
    connect(this->_worker, &ImagesModelWorker::scanComplete, this, &ImagesModel::integrateDirectory);

    foreach (const QString &path, ImagesModel::_imagesdirpaths)
    {
        emit dirNeedsScan(path, ImagesModel::_filterlist);
    }
}

ImagesModel::~ImagesModel()
{
    this->_worker->deleteLater();
    this->_workerthread->quit();
}

QHash<int, QByteArray> ImagesModel::roleNames() const
{
    QHash<int, QByteArray> hash;

    hash.insert(ImagesModel::PathRole, QByteArray("path"));
    hash.insert(ImagesModel::DateRole, QByteArray("date"));
    hash.insert(ImagesModel::OrientationRole, QByteArray("orientation"));
    hash.insert(ImagesModel::UrlRole, QByteArray("url"));

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

    if (emitReset)
        endResetModel();
}

void ImagesModel::integrateDirectory(ImagesModel::EntryList list)
{
    foreach (const ImagesModel::Entry &entry, list)
    {
        int i = 0;

        switch (this->_sortorder)
        {
            case Qt::AscendingOrder:
            {
                while (i < this->_entries.size() && biggerThan(entry, this->_entries.at(i), this->_sortrole))
                {
                    i++;
                }

                beginInsertRows(QModelIndex(), i, i);
                this->_entries.insert(i, entry);
                endInsertRows();

                break;
            }

            case Qt::DescendingOrder:
            {
                while (i < this->_entries.size() && lesserThan(entry, this->_entries.at(i), this->_sortrole))
                {
                    i++;
                }

                beginInsertRows(QModelIndex(), i, i);
                this->_entries.insert(i, entry);
                endInsertRows();

                break;
            }
        }
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
    }

    return false;
}

#include "moc_imagesmodel.cpp"
