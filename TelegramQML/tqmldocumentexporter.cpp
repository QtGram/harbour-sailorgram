#include "tqmldocumentexporter.h"
#include "telegramqmlinitializer.h"

#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QDir>

#include <telegram/objects/messagesfilterobject.h>

class MessagesFilterObj : public MessagesFilterObject
{
public:
    static QStringList requiredProperties() {
        return QStringList();
    }
};

class TqmlDocumentExporterPrivate
{
public:
    QString destination;
    QStringList indexCache;
};

TqmlDocumentExporter::TqmlDocumentExporter(QObject *parent) :
    QObject(parent)
{
    p = new TqmlDocumentExporterPrivate;
}

TqmlDocumentExporter::~TqmlDocumentExporter()
{
    delete p;
}

void TqmlDocumentExporter::setDestination(const QString &destination)
{
    if(p->destination == destination)
        return;

    p->destination = destination;
    Q_EMIT destinationChanged();
}

QString TqmlDocumentExporter::destination() const
{
    return p->destination;
}

void TqmlDocumentExporter::exportDocuments()
{
    TelegramQmlInitializer::exportDocuments(p->destination);
}
