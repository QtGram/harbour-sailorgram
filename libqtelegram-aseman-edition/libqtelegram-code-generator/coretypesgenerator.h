#ifndef CORETYPEGENERATOR_H
#define CORETYPEGENERATOR_H

#include "abstractgenerator.h"

#include <QStringList>
#include <QRegExp>
#include <QMap>
#include <QSet>

class CoreTypesGenerator : public AbstractGenerator
{
public:
    CoreTypesGenerator(const QString &dest);
    ~CoreTypesGenerator();

    void extract(const QString &data, const QString &layerVersion);

protected:
    void write(const QMap<QString,QString> &map, const QString &layerVersion);
    void writePri();

private:
    QString m_dst;
};

#endif // CORETYPEGENERATOR_H
