#ifndef APIGENERATOR_H
#define APIGENERATOR_H

#include "abstractgenerator.h"

class ApiGenerator : public AbstractGenerator
{
public:
    ApiGenerator(const QString &dst);
    ~ApiGenerator();

    void extract(const QString &data);

protected:
    void writeHeader(const QMap<QString, QList<GeneratorTypes::FunctionStruct> > &functions);
    void writeCpp(const QMap<QString, QList<GeneratorTypes::FunctionStruct> > &functions);

private:
    QString m_dst;
};

#endif // APIGENERATOR_H
