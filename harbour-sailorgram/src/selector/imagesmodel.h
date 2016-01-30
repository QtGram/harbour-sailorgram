#ifndef IMAGESMODEL_H
#define IMAGESMODEL_H


#include <QAbstractListModel>


class ImagesModelWorker;
class QThread;

class ImagesModel : public QAbstractListModel
{
    friend class ImagesModelWorker;

    Q_OBJECT
    Q_ENUMS(Role)
    Q_PROPERTY(ImagesModel::Role sortRole READ sortRole WRITE setSortRole NOTIFY sortRoleChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
    Q_PROPERTY(bool directoriesFirst READ directoriesFirst WRITE setDirectoriesFirst NOTIFY directoriesFirstChanged)
    Q_PROPERTY(QString rootDir READ rootDir WRITE setRootDir NOTIFY rootDirChanged)
    Q_PROPERTY(bool recursive READ recursive WRITE setRecursive NOTIFY recursiveChanged)

public:

    enum Role { PathRole = 0, DateRole = 1, OrientationRole = 2 , UrlRole = 3, IsDirRole = 4, NameRole = 5 };

    struct Entry
    {
        QString path;
        qint64 date;
        int orientation;
        bool isDir;
    };
    typedef QList<Entry> EntryList;

    struct Request
    {
        ImagesModel::Role sortRole = ImagesModel::DateRole;
        Qt::SortOrder sortOrder = Qt::DescendingOrder;
        bool directoriesFirst = true;
        bool recursive = false;
        QString rootDir;

        bool operator==(const Request &r2) const
        {
            return directoriesFirst == r2.directoriesFirst &&
                   recursive == r2.recursive &&
                   sortOrder == r2.sortOrder &&
                   sortRole == r2.sortRole &&
                   rootDir == r2.rootDir;
        }
    };

    explicit ImagesModel(QObject *parent = Q_NULLPTR);
    ~ImagesModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    ImagesModel::Role sortRole() const { return this->_request.sortRole; }
    Qt::SortOrder sortOrder() const { return this->_request.sortOrder; }
    bool directoriesFirst() const { return this->_request.directoriesFirst; }
    QString rootDir() const { return this->_request.rootDir; }
    bool recursive() const { return this->_request.recursive; }
    void setSortRole(ImagesModel::Role);
    void setSortOrder(Qt::SortOrder);
    void setDirectoriesFirst(bool);
    void setRootDir(const QString&);
    void setRecursive(bool);

signals:

    void sortRoleChanged();
    void sortOrderChanged();
    void directoriesFirstChanged();
    void rootDirChanged();
    void recursiveChanged();
    void newRequest(ImagesModel::Request);

private:

    ImagesModelWorker *_worker;
    QThread *_workerthread;
    ImagesModel::EntryList _entries;
    ImagesModel::Request _request;

    static const QStringList _imagesdirpaths;
    static const QStringList _filterlist;

private slots:

    void handleCompletedRequest(const ImagesModel::Request &request, const ImagesModel::EntryList &list);
};

Q_DECLARE_METATYPE(ImagesModel::Entry)
Q_DECLARE_METATYPE(ImagesModel::Request)


#endif // IMAGESMODEL_H
