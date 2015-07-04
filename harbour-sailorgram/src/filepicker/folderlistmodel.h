#ifndef FOLDERLISTMODEL_H
#define FOLDERLISTMODEL_H

#include <QAbstractListModel>
#include <QMimeDatabase>
#include <QStandardPaths>
#include <QList>
#include <QVariant>
#include <QFileInfo>
#include <QDir>

class FolderListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString directory READ directory WRITE setDirectory NOTIFY directoryChanged)
    Q_PROPERTY(QString homeFolder READ homeFolder CONSTANT FINAL)
    Q_PROPERTY(QString sdcardFolder READ sdcardFolder CONSTANT FINAL)
    Q_PROPERTY(QString mime READ mime WRITE setMime NOTIFY mimeChanged)
    Q_PROPERTY(QString directoryName READ directoryName NOTIFY directoryNameChanged)

    public:
        enum Roles { FileIconRole = Qt::UserRole + 1, FileNameRole = Qt::UserRole + 2, FilePathRole = Qt::UserRole + 3,
                     IsFileRole = Qt::UserRole + 4, IsFolderRole = Qt::UserRole + 5, IsImageRole = Qt::UserRole + 6 };

    public:
        explicit FolderListModel(QObject *parent = 0);
        QString directoryName() const;
        QString homeFolder() const;
        QString sdcardFolder() const;
        QString directory() const;
        void setDirectory(const QString& dir);
        const QString& mime() const;
        void setMime(const QString& f);

    private:
        void readDirectory();

    public: /* Overriden Methods */
        virtual int rowCount(const QModelIndex &parent) const;
        virtual QVariant data(const QModelIndex &index, int role) const;
        QHash<int, QByteArray> roleNames() const;

    signals:
        void directoryChanged();
        void directoryNameChanged();
        void mimeChanged();

    private:
        QDir _directory;
        QString _mimefilter;
        QFileInfoList _files;
        QMimeDatabase _mimedb;
};

#endif // FOLDERLISTMODEL_H
