#ifndef TELEGRAMCOREGENERATOR_H
#define TELEGRAMCOREGENERATOR_H

#include "abstractgenerator.h"


class TelegramCoreGenerator : public AbstractGenerator
{
public:
    TelegramCoreGenerator(const QString &dst);
    ~TelegramCoreGenerator();

    void extract(const QString &data);

protected:
    void writeHeader(const QMap<QString, QList<GeneratorTypes::FunctionStruct> > &functions);
    void writeGlobals(const QMap<QString, QList<GeneratorTypes::FunctionStruct> > &functions);
    void writeCpp(const QMap<QString, QList<GeneratorTypes::FunctionStruct> > &functions);

private:
    QString m_dst;
};

#endif // TELEGRAMCOREGENERATOR_H
