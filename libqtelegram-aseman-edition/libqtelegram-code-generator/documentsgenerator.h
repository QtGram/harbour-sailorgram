#ifndef DOCUMENTSGENERATOR_H
#define DOCUMENTSGENERATOR_H

#include "abstractgenerator.h"

class DocumentsGenerator : public AbstractGenerator
{
public:
    DocumentsGenerator(const QString &dest);
    ~DocumentsGenerator();

    void extract(const QString &data);

private:
    void extractTypes(const QString &data);
    void extractMethods(const QString &data);

    void writeType(const QString &name, const QList<GeneratorTypes::FunctionStruct> &types);
    void writeType(const QString &name, const QList<GeneratorTypes::TypeStruct> &types);

    QString typeToHtml(const GeneratorTypes::QtTypeStruct &t, const QString &prePath = QString());

private:
    QString m_dst;
};

#endif // DOCUMENTSGENERATOR_H
