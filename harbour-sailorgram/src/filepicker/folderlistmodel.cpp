#include "folderlistmodel.h"

FolderListModel::FolderListModel(QObject *parent): QAbstractListModel(parent)
{
}

QString FolderListModel::directoryName() const
{
    return this->_directory.dirName();
}

QString FolderListModel::directory() const
{
    return this->_directory.path();
}

void FolderListModel::setDirectory(const QString &path)
{
    if(this->_directory.path() == path)
        return;

    this->_directory = QDir(path);

    emit directoryChanged();
    emit directoryNameChanged();

    if(!this->_directory.path().isEmpty())
        this->readDirectory();
}

const QString& FolderListModel::mime() const
{
    return this->_mimefilter;
}

void FolderListModel::setMime(const QString& f)
{
    if(this->_mimefilter == f)
        return;

    this->_mimefilter = f;
    emit mimeChanged();

    if(!this->_directory.path().isEmpty())
        this->readDirectory();
}

QString FolderListModel::homeFolder() const
{
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
}

void FolderListModel::readDirectory()
{
    this->beginResetModel();

    QFileInfoList fileinfolist = this->_directory.entryInfoList(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::DirsFirst);

    if(!this->_mimefilter.isEmpty())
    {
        QMimeDatabase mimedb;
        QStringList mimefilter = this->_mimefilter.split("/");
        this->_files.clear();

        foreach(const QFileInfo& fi, fileinfolist)
        {
            if(fi.isDir())
            {
                this->_files.append(fi);
                continue;
            }

            QMimeType mime = mimedb.mimeTypeForFile(fi.filePath());
            QStringList mimestring = mime.name().split("/");

            if(mimefilter[0] != mimestring[0])
                continue;

            if((mimefilter.length() > 1) && mimefilter[1] != mimestring[1])
                continue;

            this->_files.append(fi);
        }
    }
    else
        this->_files = fileinfolist;

    this->endResetModel();
}

int FolderListModel::rowCount(const QModelIndex&) const
{
    return this->_files.length();
}

QVariant FolderListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() >= this->_files.length())
        return QVariant();

    const QFileInfo& fi = this->_files.at(index.row());

    switch(role)
    {
        case FolderListModel::FileIconRole:
            return (fi.isFile() ? "image://theme/icon-m-document" : "image://theme/icon-m-folder");

        case FolderListModel::FileNameRole:
            return fi.fileName();

        case FolderListModel::FilePathRole:
            return fi.filePath();

        case FolderListModel::IsFileRole:
            return fi.isFile();

        case FolderListModel::IsFolderRole:
            return fi.isDir();

        case FolderListModel::IsImageRole:
            return fi.isFile() && this->_mimedb.mimeTypeForFile(fi.filePath()).name().split("/")[0] == "image";

        default:
            break;
    }

    return QVariant();
}

QHash<int, QByteArray> FolderListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FolderListModel::FileIconRole] = "fileicon";
    roles[FolderListModel::FileNameRole] = "filename";
    roles[FolderListModel::FilePathRole] = "filepath";
    roles[FolderListModel::IsFileRole] = "isfile";
    roles[FolderListModel::IsFolderRole] = "isfolder";
    roles[FolderListModel::IsImageRole] = "isimage";


    return roles;
}
