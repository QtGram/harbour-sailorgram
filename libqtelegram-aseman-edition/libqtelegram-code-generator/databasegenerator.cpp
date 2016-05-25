#include "databasegenerator.h"

#include <QDir>
#include <QMap>
#include <QFile>
#include <QDebug>

DatabaseGenerator::DatabaseGenerator(const QString &dest) :
    m_dst(dest)
{

}

void DatabaseGenerator::extract(const QString &data)
{
    QDir().mkpath(m_dst);
    extractTypes(data);
}

void DatabaseGenerator::extractTypes(const QString &data)
{
    const QString tdst = m_dst + "/";
    QString result;

    int count = 0;
    QMap<QString, QList<GeneratorTypes::TypeStruct> > types = AbstractGenerator::extractTypes(data, QString(), "telegram/types/", "types");
    QMapIterator<QString, QList<GeneratorTypes::TypeStruct> > i(types);
    while(i.hasNext())
    {
        i.next();
        const QString &name = i.key();
        if(name.contains(".") ||
           name.contains("Input") ||
           name.contains("Auth") ||
           name.contains("Account") ||
           name.contains("Encrypted") ||
           name.contains("Dc") ||
           name.contains("Update") ||
           name.contains("Bot") ||
           name.contains("Config") ||
           name.contains("Keyboard"))
            continue;

        const QList<GeneratorTypes::TypeStruct> &types = i.value();
        result += convertTypeToSql(name, types);
        count++;
    }

    qDebug() << count;
    QFile file(tdst + "/database.sql");
    if(!file.open(QFile::WriteOnly))
        return;

    file.write(result.toUtf8());
    file.close();
}

QString DatabaseGenerator::convertTypeToSql(const QString &name, const QList<GeneratorTypes::TypeStruct> &types)
{
    const QString &clssName = classCaseType(name);

    QString result;
    result += QString("CREATE TABLE IF NOT EXISTS %1 (\n").arg(clssName);

    QString resultIndexes;
    QString resultArgs;

    QString defaultType;
    QMap<QString, QMap<QString,GeneratorTypes::ArgStruct> > properties;
    for(int i=0; i<types.count(); i++)
    {
        const GeneratorTypes::TypeStruct &t = types[i];
        if(defaultType.isEmpty())
            defaultType = t.typeName;
        else
            if(t.typeName.contains("Empty") || t.typeName.contains("Invalid"))
                defaultType = t.typeName;

        for(int j=0; j<t.args.length(); j++)
        {
            const GeneratorTypes::ArgStruct &arg = t.args[j];
            if(properties.contains(arg.argName) && properties[arg.argName].contains(arg.type.name))
                continue;

            properties[arg.argName][arg.type.name] = arg;
        }
    }

    QList<GeneratorTypes::TypeStruct> modifiedTypes = types;

    QMapIterator<QString, QMap<QString,GeneratorTypes::ArgStruct> > pi(properties);
    while(pi.hasNext())
    {
        pi.next();
        const QMap<QString,GeneratorTypes::ArgStruct> &hash = pi.value();
        QMapIterator<QString,GeneratorTypes::ArgStruct> mi(hash);
        while(mi.hasNext())
        {
            mi.next();
            const GeneratorTypes::ArgStruct &arg = mi.value();
            QString argName = arg.argName;
            if(properties[pi.key()].count() > 1 && arg.type.name.toLower() != arg.argName.toLower())
            {
                argName = arg.argName + "_" + QString(arg.type.originalType).remove(classCaseType(arg.argName));

                for(int i=0; i<modifiedTypes.count(); i++)
                {
                    GeneratorTypes::TypeStruct &ts = modifiedTypes[i];
                    for(int i=0; i<ts.args.count(); i++)
                    {
                        GeneratorTypes::ArgStruct &secArg = ts.args[i];
                        if(secArg.argName == arg.argName &&
                           secArg.type.name == arg.type.name)
                        {
                            secArg.argName = argName;
                        }
                    }
                }
            }

            bool idx;
            const QString &cammelCase = cammelCaseType(argName);
            const QString &classCase = classCaseType(argName);
            const GeneratorTypes::QtTypeStruct &type = arg.type;
            const QString inputType = translateType(type.name, idx);

            if(!resultArgs.isEmpty())
                resultArgs += ", \n";
            resultArgs += QString("%1 %2").arg(cammelCase).arg(inputType);

            if(idx)
                resultIndexes += QString("CREATE INDEX IF NOT EXISTS \"%1.%2_idx\" ON \"%1\"(\"%2\");\n").arg(clssName, argName);
        }
    }
    if(resultArgs.isEmpty())
        return "";

    result += resultArgs + "\n);\n" + resultIndexes + "\n\n";
    return result;
}

QString DatabaseGenerator::translateType(const QString &type, bool &idx)
{
    idx = false;
    if(type == "QString")
        return "TEXT";
    else
    if(type.contains("qint64"))
    {
        idx = true;
        return "BIGINT";
    }
    else
    if(type.contains("bool"))
    {
        idx = true;
        return "BOOLEAN";
    }
    else
    if(type.contains("int"))
    {
        idx = true;
        return "INTEGER";
    }
    else
    if(type.contains("QByteArray"))
        return "BLOB";
    else
        return "TEXT";
}

DatabaseGenerator::~DatabaseGenerator()
{
}

