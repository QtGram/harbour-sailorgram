#include "lqtgtypegenerator.h"

#include <QStringList>
#include <QRegExp>
#include <QMap>
#include <QFile>
#include <QDir>
#include <QDebug>

LqtgTypeGenerator::LqtgTypeGenerator(const QString &dest, bool inlineMode) :
    m_dst(dest),
    m_inlineMode(inlineMode)
{

}

LqtgTypeGenerator::~LqtgTypeGenerator()
{

}

void LqtgTypeGenerator::writeTypeHeader(const QString &name, const QList<GeneratorTypes::TypeStruct> &types)
{
    const QString &clssName = classCaseType(name);

    QString result;
    result += QString("class LIBQTELEGRAMSHARED_EXPORT %1 : public TelegramCustomTypeObject\n{\npublic:\n"
                      "    enum %1ClassType {\n").arg(clssName);

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

        result += "        " + t.typeName + " = " + t.typeCode;
        if(i < types.count()-1)
            result += ",\n";
        else
            result += "\n    };\n\n";

        for(int j=0; j<t.args.length(); j++)
        {
            const GeneratorTypes::ArgStruct &arg = t.args[j];
            if(properties.contains(arg.argName) && properties[arg.argName].contains(arg.type.name))
                continue;

            properties[arg.argName][arg.type.name] = arg;
        }
    }

    result += QString("    %1(%1ClassType classType = %2);\n").arg(clssName, defaultType);
    result += QString("    %1(const Null&);\n    virtual ~%1();\n\n").arg(clssName);

    QString privateResult = "private:\n";
    QString includes = "#include \"telegramcustomtypeobject.h\"\n\n#include <QMetaType>\n";
    QSet<QString> addedIncludes;

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
                argName = arg.argName + "_" + QString(arg.type.originalType).remove(classCaseType(arg.argName));

            const QString &cammelCase = cammelCaseType(argName);
            const QString &classCase = classCaseType(argName);
            const GeneratorTypes::QtTypeStruct &type = arg.type;
            const QString inputType = type.constRefrence? "const " + type.name + " &" : type.name + " ";

            result += QString("    void set%1(%2%3);\n").arg(classCase, inputType, cammelCase);
            result += QString("    %2 %1() const;\n\n").arg(cammelCase, type.name);

            foreach(const QString &inc, arg.type.includes)
            {
                if(addedIncludes.contains(inc))
                    continue;

                includes += inc + "\n";
                addedIncludes.insert(inc);
            }

            if(!arg.flagDedicated)
                privateResult += QString("    %1 m_%2;\n").arg(type.name, cammelCase);
        }
    }
    privateResult += QString("    %1ClassType m_classType;\n").arg(clssName);

    result += QString("    void setClassType(%1ClassType classType);\n    %1ClassType classType() const;\n\n").arg(clssName);
    result += QString("    bool operator ==(const %1 &b) const;\n\n").arg(clssName);
    result += "    bool operator==(bool stt) const { return isNull() != stt; }\n"
              "    bool operator!=(bool stt) const { return !operator ==(stt); }\n\n";
    result += privateResult + QString("};\n\nQ_DECLARE_METATYPE(%1)\n\n").arg(clssName);

    result = includes + "\n" + result;
    result = QString("#ifndef LQTG_TYPE_%1\n#define LQTG_TYPE_%1\n\n").arg(clssName.toUpper()) + result;
    result += QString("#endif // LQTG_TYPE_%1\n").arg(clssName.toUpper());

    QFile file(m_dst + "/" + clssName.toLower() + ".h");
    if(!file.open(QFile::WriteOnly))
        return;

    file.write(GENERATOR_SIGNATURE("//"));
    file.write(result.toUtf8());
    file.close();
}

void LqtgTypeGenerator::writeTypeClass(const QString &name, const QList<GeneratorTypes::TypeStruct> &types)
{
    const QString &clssName = classCaseType(name);

    QString result;
    result += QString("#include \"%1.h\"\n").arg(clssName.toLower()) +
        "#include \"../coretypes.h\"\n\n";

    QString resultTypes;
    QString resultEqualOperator = "return m_classType == b.m_classType";

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

    QString functions;
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

            const QString &cammelCase = cammelCaseType(argName);
            const QString &classCase = classCaseType(argName);
            const GeneratorTypes::QtTypeStruct &type = arg.type;
            const QString inputType = type.constRefrence? "const " + type.name + " &" : type.name + " ";

            if(!arg.flagDedicated)
            {
                if(!type.defaultValue.isEmpty())
                    resultTypes += QString("    m_%1(%2),\n").arg(cammelCase, type.defaultValue);
                resultEqualOperator += QString(" &&\n       m_%1 == b.m_%1").arg(cammelCase);
            }

            if(arg.flagDedicated)
            {
                functions += QString("void %1::set%2(%3%4) {\n    if(%4) m_%5 = (m_%5 | (1<<%6));\n"
                                     "    else m_%5 = (m_%5 & ~(1<<%6));\n}\n\n").arg(clssName, classCase, inputType, cammelCase, arg.flagName).arg(arg.flagValue);
                functions += QString("%3 %1::%2() const {\n    return (m_%4 & 1<<%5);\n}\n\n").arg(clssName, cammelCase, type.name, arg.flagName).arg(arg.flagValue);
            }
            else
            {
                functions += QString("void %1::set%2(%3%4) {\n    m_%4 = %4;\n}\n\n").arg(clssName, classCase, inputType, cammelCase);
                functions += QString("%3 %1::%2() const {\n    return m_%2;\n}\n\n").arg(clssName, cammelCase, type.name);
            }
        }
    }
    resultEqualOperator += ";";

    result += QString("%1::%1(%1ClassType classType) :\n").arg(clssName);
    result += resultTypes + QString("    m_classType(classType)\n");;
    result += "{\n}\n\n";
    result += QString("%1::%1(const Null &null) :\n    TelegramCustomTypeObject(null),\n").arg(clssName);
    result += resultTypes + QString("    m_classType(%1)\n").arg(defaultType);
    result += "{\n}\n\n";
    result += QString("%1::~%1() {\n}\n\n").arg(clssName);
    result += functions;
    result += QString("bool %1::operator ==(const %1 &b) const {\n%2}\n\n").arg(clssName, shiftSpace(resultEqualOperator, 1));

    result += QString("void %1::setClassType(%1::%1ClassType classType) {\n    m_classType = classType;\n}\n\n").arg(clssName);
    result += QString("%1::%1ClassType %1::classType() const {\n    return m_classType;\n}\n\n").arg(clssName);

    QFile file(m_dst + "/" + clssName.toLower() + ".cpp");
    if(!file.open(QFile::WriteOnly))
        return;

    file.write(GENERATOR_SIGNATURE("//"));
    file.write(result.toUtf8());
    file.close();
}

void LqtgTypeGenerator::writeType(const QString &name, const QList<GeneratorTypes::TypeStruct> &types)
{
    writeTypeHeader(name, types);
    writeTypeClass(name, types);
}

void LqtgTypeGenerator::writePri(const QStringList &types)
{
    QString result = "\n";
    QString headers = "HEADERS += \\\n    $$PWD/customtypes.h \\\n    $$PWD/telegramcustomtypeobject.h \\\n";
    QString sources = "SOURCES += \\\n    $$PWD/telegramcustomtypeobject.cpp \\\n";
    for(int i=0; i<types.count(); i++)
    {
        const QString &t = classCaseType(types[i]);
        const bool last = (i == types.count()-1);

        headers += QString(last? "    $$PWD/%1.h\n" : "    $$PWD/%1.h \\\n").arg(t.toLower());
        sources += QString(last? "    $$PWD/%1.cpp\n" : "    $$PWD/%1.cpp \\\n").arg(t.toLower());
    }

    result += headers + "\n";
    result += sources + "\n";
//    result = result.replace("$$PWD","telegram/types");

    QFile file(m_dst + "/customtypes.pri");
    if(!file.open(QFile::WriteOnly))
        return;

    file.write(GENERATOR_SIGNATURE("#"));
    file.write(result.toUtf8());
    file.close();
}

void LqtgTypeGenerator::writeMainHeader(const QStringList &types)
{
    QString result = "\n#include \"telegramcustomtypeobject.h\"\n";
    for(int i=0; i<types.count(); i++)
    {
        const QString &t = classCaseType(types[i]);
        result += QString("#include \"%1.h\"\n").arg(t.toLower());
    }

    QFile file(m_dst + "/customtypes.h");
    if(!file.open(QFile::WriteOnly))
        return;

    file.write(GENERATOR_SIGNATURE("//"));
    file.write(result.toUtf8());
    file.close();
}

void LqtgTypeGenerator::copyEmbeds()
{
    const QString &fcpp = m_dst + "/telegramcustomtypeobject.cpp";
    QFile::remove(fcpp);
    QFile::copy(":/embeds/telegramcustomtypeobject.cpp", fcpp);
    QFile(fcpp).setPermissions(QFileDevice::ReadUser|QFileDevice::WriteUser|
                                  QFileDevice::ReadGroup|QFileDevice::WriteGroup);

    const QString &fh = m_dst + "/telegramcustomtypeobject.h";
    QFile::remove(fh);
    QFile::copy(":/embeds/telegramcustomtypeobject.h", fh);
    QFile(fh).setPermissions(QFileDevice::ReadUser|QFileDevice::WriteUser|
                                  QFileDevice::ReadGroup|QFileDevice::WriteGroup);
}

void LqtgTypeGenerator::extract(const QString &data)
{
    QDir().mkpath(m_dst);

    QMap<QString, QList<GeneratorTypes::TypeStruct> > types = extractTypes(data, QString(), "telegram/types/", "customtypes");
    QMapIterator<QString, QList<GeneratorTypes::TypeStruct> > i(types);
    while(i.hasNext())
    {
        i.next();
        const QString &name = i.key();
        const QList<GeneratorTypes::TypeStruct> &types = i.value();
        writeType(name, types);
    }

    writePri(types.keys());
    writeMainHeader(types.keys());
    copyEmbeds();
}

