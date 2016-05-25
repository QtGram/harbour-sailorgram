#include "abstractgenerator.h"

QString AbstractGenerator::shiftSpace(const QString &str, int level)
{
    QString space;
    for(int i=0; i<level; i++)
        space += "    ";

    return space + QString(str).replace("\n", "\n" + space).trimmed() + "\n";
}

QString AbstractGenerator::fixDeniedNames(const QString &str)
{
    if(str == "long")
        return "longValue";
    else
    if(str == "private")
        return "privateValue";
    else
    if(str == "public")
        return "publicValue";
    else
        return str;
}

QString AbstractGenerator::fixTypeName(const QString &str)
{
    QString result = str;
    for(int i=0; i<result.length(); i++)
    {
        QChar ch = result[i];
        if(ch == '.')
        {
            result.remove(i,1);
            result[i] = result[i].toUpper();
        }
    }

    return result;
}

QString AbstractGenerator::cammelCaseType(const QString &str)
{
    QString result = fixTypeName(str);
    for(int i=0; i<result.length(); i++)
    {
        QChar ch = result[i];
        if(ch == '_')
        {
            result.remove(i,1);
            result[i] = result[i].toUpper();
        }
    }

    return result;
}

QString AbstractGenerator::usCaseType(const QString &str)
{
    QString result = fixTypeName(str);
    for(int i=0; i<result.length(); i++)
    {
        QChar ch = result[i];
        if(ch.isUpper())
        {
            result.replace(i, 1, QString("_") + ch.toLower());
            i++;
        }
    }

    return result;
}

QString AbstractGenerator::undoCase(const QString &str)
{
    QString result = classCaseType(str);
    for(int i=0; i<result.length(); i++)
    {
        QChar ch = result[i];
        if(ch.isUpper() && i!=0)
        {
            result.replace(i, 1, QString(" ") + ch);

            while(i+1<result.length() && result[i+1].isUpper())
            {
                result[i+1] = result[i+1].toLower();
                i++;
            }
        }
    }

    return result;
}

QString AbstractGenerator::classCaseType(const QString &str)
{
    QString result = cammelCaseType(str);
    if(!result.isEmpty())
        result[0] = result[0].toUpper();

    return result;
}

QString AbstractGenerator::unclassCaseType(const QString &str)
{
    QString result = cammelCaseType(str);
    if(!result.isEmpty())
        result[0] = result[0].toLower();

    return result;
}

GeneratorTypes::QtTypeStruct AbstractGenerator::translateType(const QString &type, bool addNameSpace, const QString &prePath, const QString &postPath)
{
    GeneratorTypes::QtTypeStruct result;
    result.originalType = type;

    if(type == "true")
    {
        result.name = "bool";
        result.defaultValue = "false";
    }
    else
    if(type == "int")
    {
        result.name = "qint32";
        result.includes << "#include <QtGlobal>";
        result.defaultValue = "0";
    }
    else
    if(type == "Bool")
    {
        result.name = "bool";
        result.defaultValue = "false";
    }
    else
    if(type == "long")
    {
        result.name = "qint64";
        result.defaultValue = "0";
        result.includes << "#include <QtGlobal>";
    }
    else
    if(type == "double")
    {
        result.name = "qreal";
        result.defaultValue = "0";
        result.includes << "#include <QtGlobal>";
    }
    else
    if(type == "string")
    {
        result.name = "QString";
        result.includes << "#include <QString>";
        result.constRefrence = true;
    }
    else
    if(type == "bytes")
    {
        result.name = "QByteArray";
        result.includes << "#include <QByteArray>";
        result.constRefrence = true;
    }
    else
    if(type.contains("Vector<"))
    {
        GeneratorTypes::QtTypeStruct innerType = translateType(type.mid(7,type.length()-8), addNameSpace, prePath);

        result.includes << "#include <QList>";
        result.includes << innerType.includes;
        result.name = QString("QList<%1>").arg(innerType.name);
        result.constRefrence = true;
        result.isList = true;
        result.innerType = new GeneratorTypes::QtTypeStruct(innerType);
    }
    else
    {
        result.name = QString(addNameSpace? "Types::" : "") + classCaseType(type);
        result.includes << QString("#include \"%1\"").arg(prePath + classCaseType(type).toLower() + postPath + ".h");
        result.constRefrence = true;
        result.qtgType = true;
    }

    result.html = QString(result.name).replace("<", "&lt;").replace(">", "&gt;");
    return result;
}

QMap<QString, QList<GeneratorTypes::TypeStruct> > AbstractGenerator::extractTypes(const QString &data, const QString &objectsPostPath, const QString &objectsPrePath, const QString &customHeader)
{
    QMap<QString, QList<GeneratorTypes::TypeStruct> > types;
    const QStringList &lines = QString(data).split("\n",QString::SkipEmptyParts);
    bool hasAccess = false;
    foreach(const QString &line, lines)
    {
        const QString &l = line.trimmed();
        if(l.left(3) == "---")
        {
            if(l == QString("---%1---").arg(customHeader) )
                hasAccess = true;
            else
                hasAccess = false;

            continue;
        }
        if(!hasAccess)
            continue;

        const QStringList &parts = l.split(" ", QString::SkipEmptyParts);
        if(parts.count() < 3)
            continue;

        const QString signature = parts.first();
        const int signKeyIndex = signature.indexOf("#");
        const QString name = signature.mid(0,signKeyIndex);
        const QString code = signature.mid(signKeyIndex+1);
        QString structName = QString(parts.last()).remove(";");
        if(structName == "Updates")
            structName = "UpdatesType";

        const QStringList args = parts.mid(1, parts.count()-3);

        GeneratorTypes::TypeStruct type;
        foreach(const QString &str, args)
        {
            GeneratorTypes::ArgStruct arg;
            int splitterIdx = str.indexOf(":");

            arg.argName = fixDeniedNames(str.mid(0,splitterIdx));

            QString typePart = str.mid(splitterIdx+1);
            if(typePart == "#")
            {
                typePart = "int";
                arg.isFlag = true;
            }

            int ifIdx = typePart.indexOf("?");
            bool hasIf = (ifIdx != -1);
            arg.type = translateType(hasIf? typePart.mid(ifIdx+1) : typePart, false, objectsPrePath, objectsPostPath);
            if(hasIf)
            {
                QString flagsPart = typePart.mid(0,ifIdx);
                int flagSplitter = flagsPart.indexOf(".");
                if(flagSplitter != -1)
                {
                    arg.flagName = flagsPart.mid(0,flagSplitter);
                    arg.flagValue = flagsPart.mid(flagSplitter+1).toInt();
                    arg.flagDedicated = (typePart.mid(ifIdx+1)=="true");
                }
            }

            type.args << arg;
        }

        type.typeName = "type" + classCaseType(name);
        type.typeCode = "0x" + code;
        type.code = l;

        types[structName] << type;
    }

    return types;
}

QMap<QString, QList<GeneratorTypes::FunctionStruct> > AbstractGenerator::extractFunctions(const QString &data)
{
    QMap<QString, QList<GeneratorTypes::FunctionStruct> > functions;
    const QStringList &lines = QString(data).split("\n",QString::SkipEmptyParts);
    bool hasAccess = false;
    foreach(const QString &line, lines)
    {
        const QString &l = line.trimmed();
        if(l.left(3) == "---")
        {
            if(l == "---functions---")
                hasAccess = true;
            else
                hasAccess = false;

            continue;
        }
        if(!hasAccess)
            continue;

        const QStringList &parts = l.split(" ", QString::SkipEmptyParts);
        if(parts.count() < 3)
            continue;

        const QString signature = parts.first();
        const int signKeyIndex = signature.indexOf("#");
        const QString name = signature.mid(0,signKeyIndex);
        const int pointIndex = name.indexOf(".");
        if(pointIndex < 1)
            continue;

        const QString fileName = name.mid(0, pointIndex);
        const QString className = classCaseType(fileName);
        const QString functionName = name.mid(pointIndex+1);
        const QString code = signature.mid(signKeyIndex+1);
        QString returnType = QString(parts.last()).remove(";");
        if(returnType == "Updates")
            returnType = "UpdatesType";
        const QStringList args = parts.mid(1, parts.count()-3);

        GeneratorTypes::FunctionStruct fnc;
        foreach(const QString &str, args)
        {
            GeneratorTypes::ArgStruct arg;
            int splitterIdx = str.indexOf(":");

            arg.argName = fixDeniedNames(str.mid(0,splitterIdx));

            QString typePart = str.mid(splitterIdx+1);
            if(typePart == "#")
            {
                typePart = "int";
                arg.isFlag = true;
            }

            int ifIdx = typePart.indexOf("?");
            bool hasIf = (ifIdx != -1);
            arg.type = translateType(hasIf? typePart.mid(ifIdx+1) : typePart, false, "telegram/types/");
            if(hasIf)
            {
                QString flagsPart = typePart.mid(0,ifIdx);
                int flagSplitter = flagsPart.indexOf(".");
                if(flagSplitter != -1)
                {
                    arg.flagName = flagsPart.mid(0,flagSplitter);
                    arg.flagValue = flagsPart.mid(flagSplitter+1).toInt();
                    arg.flagDedicated = (typePart.mid(ifIdx+1)=="true");
                }
            }

            fnc.type.args << arg;
        }

        fnc.type.typeName = "fnc" + classCaseType(name);
        fnc.type.typeCode = "0x" + code;
        fnc.className = className;
        fnc.functionName = functionName;
        fnc.returnType = translateType(returnType, false, "telegram/types/");
        fnc.code = l;

        functions[className] << fnc;
    }
    return functions;
}

