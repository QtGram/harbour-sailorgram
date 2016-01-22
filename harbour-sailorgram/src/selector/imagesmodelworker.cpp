#include "imagesmodelworker.h"
#include "exif/exif.h"

#include <QFileInfoList>
#include <QDir>
#include <QDateTime>


ImagesModelWorker::ImagesModelWorker(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<ImagesModel::Entry>();
}

void ImagesModelWorker::scanDirectory(const QString &dirPath, const QStringList &filterList, bool recursive)
{
    ImagesModel::EntryList entryList;

    QDir directory(dirPath);

    if (directory.exists())
    {
        QFileInfoList list = directory.entryInfoList(filterList, QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files | QDir::Readable);

        foreach (const QFileInfo &fileInfo, list)
        {
            const QString filePath(fileInfo.absoluteFilePath());

            ImagesModel::Entry entry;
            entry.path = filePath;
            entry.date = fileInfo.lastModified().toMSecsSinceEpoch();

            if (fileInfo.isDir())
            {
                if (recursive)
                    scanDirectory(filePath, filterList, recursive);
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

    emit scanComplete(entryList, dirPath);
}
