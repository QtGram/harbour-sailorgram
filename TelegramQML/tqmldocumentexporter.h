#ifndef TQMLDOCUMENTEXPORTER_H
#define TQMLDOCUMENTEXPORTER_H

#include <QObject>

class TqmlDocumentExporterPrivate;
class TqmlDocumentExporter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString destination READ destination WRITE setDestination NOTIFY destinationChanged)

public:
    TqmlDocumentExporter(QObject *parent = 0);
    ~TqmlDocumentExporter();

    void setDestination(const QString &destination);
    QString destination() const;

Q_SIGNALS:
    void destinationChanged();

public Q_SLOTS:
    void exportDocuments();

private:
    TqmlDocumentExporterPrivate *p;
};

#endif // TQMLDOCUMENTEXPORTER_H
