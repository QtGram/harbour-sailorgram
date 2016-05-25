#ifndef DATABASEGENERATOR_H
#define DATABASEGENERATOR_H

#include "abstractgenerator.h"

class DatabaseGenerator : public AbstractGenerator
{
public:
    DatabaseGenerator(const QString &dest);
    ~DatabaseGenerator();

    void extract(const QString &data);

private:
    void extractTypes(const QString &data);
    QString convertTypeToSql(const QString &name, const QList<GeneratorTypes::TypeStruct> &types);
    QString translateType(const QString &type, bool &idx);

private:
    QString m_dst;
};

#endif // DATABASEGENERATOR_H
