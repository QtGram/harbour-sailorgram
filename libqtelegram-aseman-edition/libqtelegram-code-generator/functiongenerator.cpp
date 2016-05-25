#include "functiongenerator.h"

#include <QDebug>
#include <QFile>
#include <QDir>

FunctionGenerator::FunctionGenerator(const QString &dest, bool inlineMode) :
    m_dst(dest),
    m_inlineMode(inlineMode)
{
}

FunctionGenerator::~FunctionGenerator()
{
}

QString FunctionGenerator::typeToFetchFunction(const QString &arg, const QString &type, const GeneratorTypes::ArgStruct &argStruct)
{
    QString result;
    QString baseResult;

    if(type == "qint32")
        baseResult += arg + " = in->fetchInt();";
    else
    if(type == "bool")
        baseResult += arg + " = in->fetchBool();";
    else
    if(type == "qint64")
        baseResult += arg + " = in->fetchLong();";
    else
    if(type == "qreal")
        baseResult += arg + " = in->fetchDouble();";
    else
    if(type == "QByteArray")
        baseResult += arg + " = in->fetchBytes();";
    else
    if(type == "QString")
        baseResult += arg + " = in->fetchQString();";
    else
    if(type.contains("QList<"))
    {
        QString innerType = type.mid(6,type.length()-7);
        baseResult += QString("if(in->fetchInt() != (qint32)CoreTypes::typeVector) return %1;\n").arg(arg);

        baseResult += "qint32 " + arg + "_length = in->fetchInt();\n";
        baseResult += arg + ".clear();\n";
        baseResult += "for (qint32 i = 0; i < " + arg + "_length; i++) {\n";
        baseResult += "    " + innerType + " type;\n    " + typeToFetchFunction("type", innerType, argStruct) + "\n";
        baseResult += QString("    %1.append(type);\n}").arg(arg);
    }
    else
    {
        baseResult += QString("if(!" + arg + ".fetch(in)) return %1;").arg("result");
    }

    if(!argStruct.flagName.isEmpty())
    {
        if(argStruct.flagDedicated)
            baseResult = arg + QString(" = (m_%1 & 1<<%2)").arg(argStruct.flagName).arg(argStruct.flagValue);
        else
        {
            result += QString("if(m_%1 & 1<<%2) ").arg(argStruct.flagName).arg(argStruct.flagValue);
            baseResult = "{\n" + shiftSpace(baseResult, 1) + "}";
        }
    }

    result += baseResult;
    return result;
}

QString FunctionGenerator::fetchFunction(const QString &clssName, const QString &fncName, const GeneratorTypes::ArgStruct &arg)
{
    Q_UNUSED(clssName)
    Q_UNUSED(fncName)

    QString result;
    result += QString("%1 %2;\n").arg(arg.type.name, arg.argName);
    result += typeToFetchFunction(cammelCaseType(arg.argName), arg.type.name, arg) + "\n";
    result += QString("return %1;").arg(arg.argName);
    result = shiftSpace(result, 1);
    return result;
}

QString FunctionGenerator::typeToPushFunction(const QString &arg, const QString &type, const GeneratorTypes::ArgStruct &argStruct, bool justFlagTest)
{
    QString result;

    if(type == "qint32")
        result += "out->appendInt(" + arg + ");";
    else
    if(type == "bool")
        result += "out->appendBool(" + arg + ");";
    else
    if(type == "qint64")
        result += "out->appendLong(" + arg + ");";
    else
    if(type == "qreal")
        result += "out->appendDouble(" + arg + ");";
    else
    if(type == "QByteArray")
        result += "out->appendBytes(" + arg + ");";
    else
    if(type == "QString")
        result += "out->appendQString(" + arg + ");";
    else
    if(type.contains("QList<"))
    {
        QString innerType = type.mid(6,type.length()-7);
        result += "out->appendInt(CoreTypes::typeVector);\n";

        result += "out->appendInt(" + arg + ".count());\n";
        result += "for (qint32 i = 0; i < " + arg + ".count(); i++) {\n";
        result += "    " + typeToPushFunction(arg + "[i]", innerType, argStruct, false) + "\n}";
    }
    else
    {
        result += "if(!" + arg + ".push(out)) return false;";
    }

    if(argStruct.flagDedicated || justFlagTest)
        result = QString("if(%1 != 0) %2 = (1<<%3 | %2);")
                .arg(arg + (argStruct.type.isList?".count()":""))
                .arg(argStruct.flagName).arg(argStruct.flagValue);
    else
    if(!argStruct.flagName.isEmpty())
    {
        QString justIf = QString("if(%1 & 1<<%2) ").arg(argStruct.flagName).arg(argStruct.flagValue);
        if(result.count("\n") > 0)
            result = justIf + "{\n" + shiftSpace(result, 1) + "}";
        else
            result = justIf + result;
    }

    return result;
}

QString FunctionGenerator::pushFunction(const QString &clssName, const QString &fncName, const QList<GeneratorTypes::ArgStruct> &types)
{
    QSet<QString> definedFlags;

    QString result;
    result += QString("out->appendInt(fnc%1%2);\n").arg(clssName, classCaseType(fncName));
    for(int i=0; i<2; i++)
        foreach(const GeneratorTypes::ArgStruct &arg, types)
        {
            if(i == 0 && arg.flagName.isEmpty())
                continue;
            if(i == 1 && arg.flagDedicated)
                continue;

            if(arg.isFlag && !definedFlags.contains(arg.argName))
            {
                result += QString("\nqint32 %1 = 0;").arg(arg.argName);
                definedFlags.insert(arg.argName);
            }
            if(!arg.flagName.isEmpty() && !definedFlags.contains(arg.flagName))
            {
                result += QString("\nqint32 %1 = 0;\n").arg(arg.flagName);
                definedFlags.insert(arg.flagName);
            }
            if(arg.isFlag)
                result += "\n";

            result += typeToPushFunction(cammelCaseType(arg.argName), arg.type.name, arg,
                                         i==0 ) + "\n";
        }

    result += "return true;\n";
    result = shiftSpace(result, 1);

    return result;
}

void FunctionGenerator::extract(const QString &data)
{
    QDir().mkpath(m_dst);

    QMap<QString, QList<GeneratorTypes::FunctionStruct> > functions = extractFunctions(data);
    QMapIterator<QString, QList<GeneratorTypes::FunctionStruct> > i(functions);
    while(i.hasNext())
    {
        i.next();
        const QString &name = i.key();
        const QList<GeneratorTypes::FunctionStruct> &types = i.value();
        writeType(name, types);
    }

    writePri(functions.keys());
    writeMainHeader(functions.keys());
    copyEmbeds();
}

void FunctionGenerator::writeTypeHeader(const QString &name, const QList<GeneratorTypes::FunctionStruct> &functions)
{
    const QString &clssName = classCaseType(name);

    QString result;
    result += "namespace Tg {\n";
    result += "namespace Functions {\n\n";
    result += QString("class LIBQTELEGRAMSHARED_EXPORT %1 : public TelegramFunctionObject\n{\npublic:\n"
                      "    enum %1Function {\n").arg(clssName);

    QString includes = "#include \"telegramfunctionobject.h\"\n";
    if(m_inlineMode)
    {
        includes += "#include \"core/inboundpkt.h\"\n"
                    "#include \"core/outboundpkt.h\"\n"
                    "#include \"../coretypes.h\"\n\n";
    }
    QString resultFnc;

    QSet<QString> addedIncludes;
    QMap<QString, QMap<QString,GeneratorTypes::ArgStruct> > properties;
    for(int i=0; i<functions.count(); i++)
    {
        const GeneratorTypes::FunctionStruct &f = functions[i];
        const GeneratorTypes::TypeStruct &t = f.type;

        result += "        " + t.typeName + " = " + t.typeCode;
        if(i < functions.count()-1)
            result += ",\n";
        else
            result += "\n    };\n\n";

        foreach(const QString &inc, f.returnType.includes)
        {
            if(addedIncludes.contains(inc))
                continue;
            includes += inc + "\n";
            addedIncludes.insert(inc);
        }

        QString fncArgs;
        for(int j=0; j<t.args.length(); j++)
        {
            const GeneratorTypes::ArgStruct &arg = t.args[j];
            foreach(const QString &inc, arg.type.includes)
            {
                if(addedIncludes.contains(inc))
                    continue;
                includes += inc + "\n";
                addedIncludes.insert(inc);
            }

            properties[f.functionName][arg.type.name] = arg;
            if(arg.isFlag)
                continue;

            const QString inputType = arg.type.constRefrence? "const " + arg.type.name + " &" : arg.type.name + " ";
            fncArgs += QString(", %1%2").arg(inputType, cammelCaseType(arg.argName));
        }

        resultFnc += QString("    static bool %1(OutboundPkt *out%2);\n").arg(f.functionName, fncArgs);
        resultFnc += QString("    static %2 %1Result(InboundPkt *in);\n\n").arg(f.functionName,f.returnType.name);
    }

    result += QString("    %1();\n").arg(clssName);
    result += QString("    virtual ~%1();\n\n").arg(clssName);
    result += resultFnc;

    result += "};\n\n";
    result += "}\n" + writeTypeClass(name, functions) +
              "\n}\n\n";

    result = includes + "\n" + result;
    result = QString("#ifndef LQTG_FNC_%1\n#define LQTG_FNC_%1\n\n").arg(clssName.toUpper()) + result;
    result += QString("#endif // LQTG_FNC_%1\n").arg(clssName.toUpper());

    QFile file(m_dst + "/" + clssName.toLower() + ".h");
    if(!file.open(QFile::WriteOnly))
        return;

    file.write(GENERATOR_SIGNATURE("//"));
    file.write(result.toUtf8());
    file.close();
}

QString FunctionGenerator::writeTypeClass(const QString &name, const QList<GeneratorTypes::FunctionStruct> &functions)
{
    const QString &clssName = classCaseType(name);

    QString preFnc;
    if(m_inlineMode)
        preFnc = "inline ";

    QString result;
    if(!m_inlineMode)
    {
        result += QString("#include \"%1.h\"\n").arg(clssName.toLower()) +
                "#include \"core/inboundpkt.h\"\n"
                "#include \"core/outboundpkt.h\"\n"
                "#include \"../coretypes.h\"\n\n";

        result += "using namespace Tg;\n\n";
    }

    result += preFnc + QString("Functions::%1::%1() {\n}\n\n").arg(clssName);
    result += preFnc + QString("Functions::%1::~%1() {\n}\n\n").arg(clssName);

    QString resultFnc;

    QMap<QString, QMap<QString,GeneratorTypes::ArgStruct> > properties;
    for(int i=0; i<functions.count(); i++)
    {
        const GeneratorTypes::FunctionStruct &f = functions[i];
        const GeneratorTypes::TypeStruct &t = f.type;

        GeneratorTypes::ArgStruct retArg;
        retArg.argName = "result";
        retArg.type = f.returnType;

        const QString &pushResult = pushFunction(f.className, f.functionName, t.args);
        QString fetchResult = fetchFunction(f.className, f.functionName, retArg);
        QString fncArgs;

        for(int j=0; j<t.args.length(); j++)
        {
            const GeneratorTypes::ArgStruct &arg = t.args[j];

            properties[f.functionName][arg.type.name] = arg;
            if(arg.isFlag)
                continue;

            const QString inputType = arg.type.constRefrence? "const " + arg.type.name + " &" : arg.type.name + " ";

            fncArgs += QString(", %1%2").arg(inputType, cammelCaseType(arg.argName));
        }

        resultFnc += preFnc + QString("bool Functions::%3::%1(OutboundPkt *out%2) {\n%4}\n\n").arg(f.functionName, fncArgs,clssName,pushResult);
        resultFnc += preFnc + QString("%2 Functions::%3::%1Result(InboundPkt *in) {\n%4}\n\n").arg(f.functionName,f.returnType.name,clssName,fetchResult);
    }

    result += resultFnc;

    if(m_inlineMode)
        QFile::remove(m_dst + "/" + clssName.toLower() + ".cpp");
    else
    {
        QFile file(m_dst + "/" + clssName.toLower() + ".cpp");
        if(!file.open(QFile::WriteOnly))
        {
            file.write(GENERATOR_SIGNATURE("//"));
            file.write(result.toUtf8());
            file.close();
        }
    }

    return result;
}

void FunctionGenerator::writeType(const QString &name, const QList<GeneratorTypes::FunctionStruct> &types)
{
    writeTypeHeader(name, types);
    if(m_inlineMode)
        writeTypeClass(name, types);
}

void FunctionGenerator::writePri(const QStringList &types)
{
    QString result = "\n";
    QString headers = "HEADERS += \\\n    $$PWD/functions.h \\\n    $$PWD/telegramfunctionobject.h \\\n";
    QString sources = "SOURCES += \\\n    $$PWD/telegramfunctionobject.cpp \\\n";
    for(int i=0; i<types.count(); i++)
    {
        const QString &t = classCaseType(types[i]);
        const bool last = (i == types.count()-1);

        headers += QString(last? "    $$PWD/%1.h\n" : "    $$PWD/%1.h \\\n").arg(t.toLower());
        if(!m_inlineMode)
            sources += QString(last? "    $$PWD/%1.cpp\n" : "    $$PWD/%1.cpp \\\n").arg(t.toLower());
    }

    result += headers + "\n";
    result += sources + "\n";
//    result = result.replace("$$PWD","telegram/functions");

    QFile file(m_dst + "/functions.pri");
    if(!file.open(QFile::WriteOnly))
        return;

    file.write(GENERATOR_SIGNATURE("#"));
    file.write(result.toUtf8());
    file.close();
}

void FunctionGenerator::writeMainHeader(const QStringList &types)
{
    QString result = "\n#include \"telegramfunctionobject.h\"\n";
    for(int i=0; i<types.count(); i++)
    {
        const QString &t = classCaseType(types[i]);
        result += QString("#include \"%1.h\"\n").arg(t.toLower());
    }

    QFile file(m_dst + "/functions.h");
    if(!file.open(QFile::WriteOnly))
        return;

    file.write(GENERATOR_SIGNATURE("//"));
    file.write(result.toUtf8());
    file.close();
}

void FunctionGenerator::copyEmbeds()
{
    const QString &fcpp = m_dst + "/telegramfunctionobject.cpp";
    QFile::remove(fcpp);
    QFile::copy(":/embeds/telegramfunctionobject.cpp", fcpp);
    QFile(fcpp).setPermissions(QFileDevice::ReadUser|QFileDevice::WriteUser|
                                  QFileDevice::ReadGroup|QFileDevice::WriteGroup);

    const QString &fh = m_dst + "/telegramfunctionobject.h";
    QFile::remove(fh);
    QFile::copy(":/embeds/telegramfunctionobject.h", fh);
    QFile(fh).setPermissions(QFileDevice::ReadUser|QFileDevice::WriteUser|
                                  QFileDevice::ReadGroup|QFileDevice::WriteGroup);
}

