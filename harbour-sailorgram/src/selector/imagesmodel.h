#ifndef IMAGESMODEL_H
#define IMAGESMODEL_H


#include <QAbstractListModel>


class ImagesModelWorker;
class QThread;

class ImagesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(Role)
    Q_PROPERTY(Role sortRole READ sortRole WRITE setSortRole NOTIFY sortRoleChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)

public:

    enum Role { PathRole = 0, DateRole = 1, OrientationRole = 2 , UrlRole = 3};

    struct Entry
    {
        QString path;
        qint64 date;
        int orientation;
    };
    typedef QList<Entry> EntryList;

    explicit ImagesModel(QObject *parent = Q_NULLPTR);
    ~ImagesModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    Role sortRole() const { return this->_sortrole; }
    Qt::SortOrder sortOrder() const { return this->_sortorder; }
    void setSortRole(Role);
    void setSortOrder(Qt::SortOrder);

signals:

    void sortRoleChanged();
    void sortOrderChanged();
    void dirNeedsScan(QString, QStringList);

private:

    bool lesserThan(const Entry &e1, const Entry &e2, Role role);
    bool biggerThan(const Entry &e1, const Entry &e2, Role role);

    ImagesModelWorker *_worker;
    QThread *_workerthread;
    ImagesModel::EntryList _entries;
    Role _sortrole;
    Qt::SortOrder _sortorder;

    static const QStringList _imagesdirpaths;
    static const QStringList _filterlist;

private slots:

    void sort(bool emitReset = true);
    void integrateDirectory(ImagesModel::EntryList);
};

Q_DECLARE_METATYPE(ImagesModel::Entry)


#endif // IMAGESMODEL_H
