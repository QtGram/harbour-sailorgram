#include "typeobjectgenerator.h"

#include <QStringList>
#include <QRegExp>
#include <QMap>
#include <QFile>
#include <QDir>
#include <QDebug>

TypeObjectGenerator::TypeObjectGenerator(const QString &dest, bool inlineMode) :
    m_dst(dest),
    m_inlineMode(inlineMode)
{

}

TypeObjectGenerator::~TypeObjectGenerator()
{

}

QString TypeObjectGenerator::typeToFetchFunction(const QString &arg, const QString &type, const GeneratorTypes::ArgStruct &argStruct)
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
        baseResult += "if(in->fetchInt() != (qint32)CoreTypes::typeVector) return false;\n";

        baseResult += "qint32 " + arg + "_length = in->fetchInt();\n";
        baseResult += arg + ".clear();\n";
        baseResult += "for (qint32 i = 0; i < " + arg + "_length; i++) {\n";
        baseResult += "    " + innerType + " type;\n    " + typeToFetchFunction("type", innerType, argStruct) + "\n";
        baseResult += QString("    %1.append(type);\n}").arg(arg);
    }
    else
    {
        baseResult += arg + ".fetch(in);";
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

QString TypeObjectGenerator::fetchFunction(const QString &name, const QList<GeneratorTypes::TypeStruct> &types)
{
    QString result = "LQTG_FETCH_LOG;\nint x = in->fetchInt();\nswitch(x) {\n";

    foreach(const GeneratorTypes::TypeStruct &t, types)
    {
        result += QString("case %1: {\n").arg(t.typeName);

        QString fetchPart;
        foreach(const GeneratorTypes::ArgStruct &arg, t.args)
        {
            fetchPart += typeToFetchFunction("m_" + cammelCaseType(arg.argName), arg.type.name, arg) + "\n";
        }

        fetchPart += QString("m_classType = static_cast<%1ClassType>(x);\nreturn true;\n").arg(classCaseType(name));
        result += shiftSpace(fetchPart, 1);
        result += "}\n    break;\n\n";
    }

    result += "default:\n    LQTG_FETCH_ASSERT;\n    return false;\n}\n";
    return result;
}

QString TypeObjectGenerator::typeToPushFunction(const QString &arg, const QString &type, const GeneratorTypes::ArgStruct &argStruct)
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
        result += "    " + typeToPushFunction(arg + "[i]", innerType, argStruct) + "\n}";
    }
    else
    {
        result += arg + ".push(out);";
    }

    return result;
}

QString TypeObjectGenerator::pushFunction(const QString &name, const QList<GeneratorTypes::TypeStruct> &types)
{
    Q_UNUSED(name)
    QString result = "out->appendInt(m_classType);\nswitch(m_classType) {\n";

    foreach(const GeneratorTypes::TypeStruct &t, types)
    {
        result += QString("case %1: {\n").arg(t.typeName);

        QString fetchPart;
        foreach(const GeneratorTypes::ArgStruct &arg, t.args)
        {
            if(arg.flagDedicated)
                continue;

            fetchPart += typeToPushFunction("m_" + cammelCaseType(arg.argName), arg.type.name, arg) + "\n";
        }

        fetchPart += "return true;\n";
        result += shiftSpace(fetchPart, 1);
        result += "}\n    break;\n\n";
    }

    result += "default:\n    return false;\n}\n";
    return result;
}

void TypeObjectGenerator::writeTypeHeader(const QString &name, const QList<GeneratorTypes::TypeStruct> &types)
{
    const QString &clssName = classCaseType(name);

    QString result;
    result += QString("class LIBQTELEGRAMSHARED_EXPORT %1Object : public TelegramTypeQObject\n{\n").arg(clssName);

    QString bodyResult;
    bodyResult += QString("public:\n    enum %1ClassType {\n").arg(clssName);

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

        bodyResult += "        " + classCaseType(t.typeName);
        if(i < types.count()-1)
            bodyResult += ",\n";
        else
            bodyResult += "\n    };\n\n";

        for(int j=0; j<t.args.length(); j++)
        {
            const GeneratorTypes::ArgStruct &arg = t.args[j];
            if(properties.contains(arg.argName) && properties[arg.argName].contains(arg.type.name))
                continue;

            properties[arg.argName][arg.type.name] = arg;
        }
    }

    bodyResult += QString("    %1Object(const %1 &core, QObject *parent = 0);\n").arg(clssName);
    bodyResult += QString("    %1Object(QObject *parent = 0);\n    virtual ~%1Object();\n\n").arg(clssName);

    QString privateSlotsResult = "private Q_SLOTS:\n";
    QString privateResult = "private:\n";
    QString includes = "#include \"telegramtypeqobject.h\"\n";
    includes += QString("#include \"telegram/types/%1.h\"\n\n").arg(clssName.toLower());
    includes += "#include <QPointer>\n";

    QSet<QString> addedIncludes;

    QString signalsResult = "    void coreChanged();\n    void classTypeChanged();\n";
    QString propertiesResult = QString("    Q_OBJECT\n    Q_ENUMS(%1ClassType)\n").arg(clssName);

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
            const QString objectType = arg.type.name + (arg.type.qtgType?"Object*":"");
            const QString objectInputType = (arg.type.qtgType?objectType + " ":inputType);

            bodyResult += QString("    void set%1(%2%3);\n").arg(classCase, objectInputType, cammelCase);
            bodyResult += QString("    %2 %1() const;\n\n").arg(cammelCase, objectType);
            signalsResult += QString("    void %1Changed();\n").arg(cammelCase);
            propertiesResult += QString("    Q_PROPERTY(%1 %2 READ %2 WRITE set%3 NOTIFY %2Changed)\n").arg(objectType, cammelCase, classCase);

            foreach(const QString &inc, arg.type.includes)
            {
                if(addedIncludes.contains(inc))
                    continue;

                if(arg.type.qtgType)
                    includes += inc + "\n";

                addedIncludes.insert(inc);
            }

            if(arg.type.qtgType)
            {
                privateSlotsResult += QString("    void core%1Changed();\n").arg(classCase);
                privateResult += QString("    QPointer<%1Object> m_%2;\n").arg(type.name, cammelCase);
            }
        }
    }
    propertiesResult += QString("    Q_PROPERTY(%1 core READ core WRITE setCore NOTIFY coreChanged)\n").arg(clssName)
            + "    Q_PROPERTY(quint32 classType READ classType WRITE setClassType NOTIFY classTypeChanged)\n";
    privateResult += QString("    %1 m_core;\n").arg(clssName);

    bodyResult += QString("    void setClassType(quint32 classType);\n    quint32 classType() const;\n\n");
    bodyResult += QString("    void setCore(const %1 &core);\n    %1 core() const;\n\n").arg(clssName);

    bodyResult += QString("    %1Object &operator =(const %1 &b);\n").arg(clssName);
    bodyResult += QString("    bool operator ==(const %1 &b) const;\n\n").arg(clssName);
    bodyResult += "Q_SIGNALS:\n" + signalsResult + "\n";
    bodyResult += privateSlotsResult + "\n" + privateResult + "};\n\n";

    result = includes + "\n" + result;
    result = QString("#ifndef LQTG_TYPE_%1_OBJECT\n#define LQTG_TYPE_%1_OBJECT\n\n").arg(clssName.toUpper()) + result;
    result += propertiesResult + "\n";
    result += bodyResult;
    result += writeTypeClass(name, types);
    result += QString("#endif // LQTG_TYPE_%1_OBJECT\n").arg(clssName.toUpper());

    QFile file(m_dst + "/" + clssName.toLower() + "object.h");
    if(!file.open(QFile::WriteOnly))
        return;

    file.write(GENERATOR_SIGNATURE("//"));
    file.write(result.toUtf8());
    file.close();
}

QString TypeObjectGenerator::writeTypeClass(const QString &name, const QList<GeneratorTypes::TypeStruct> &types)
{
    const QString &clssName = classCaseType(name);

    QString preFnc;
    if(m_inlineMode)
        preFnc = "inline ";

    QString result;
    QString baseResult;

    result += QString("#include \"%1object.h\"\n\n").arg(clssName.toLower());

    QString resultTypes;
    QString resultConstructorTypes;
    QString resultEqualCheckOperator = "    return m_core == b;\n";
    QString resultEqualOperator = "    if(m_core == b) return *this;\n    m_core = b;\n";
    QString resultEqualOperatorEmits;
    QString resultPrivateSlots;

    QString typeSwitch = QString("    %1::%1ClassType result;\n    switch(classType) {\n").arg(clssName);
    QString typeSwitchBack = QString("    int result;\n    switch(static_cast<qint64>(m_core.classType())) {\n");

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

        typeSwitch += QString("    case %3:\n        result = %2::%1;\n        break;\n").arg(t.typeName, clssName, classCaseType(t.typeName));
        typeSwitchBack += QString("    case %2::%1:\n        result = %3;\n        break;\n").arg(t.typeName, clssName, classCaseType(t.typeName));

        for(int j=0; j<t.args.length(); j++)
        {
            const GeneratorTypes::ArgStruct &arg = t.args[j];
            if(properties.contains(arg.argName) && properties[arg.argName].contains(arg.type.name))
                continue;

            properties[arg.argName][arg.type.name] = arg;
        }
    }
    typeSwitch += QString("    default:\n        result = %2::%1;\n        break;\n    }\n").arg(defaultType, clssName);
    typeSwitchBack += QString("    default:\n        result = %1;\n        break;\n    }\n").arg(classCaseType(defaultType));

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
            const QString objectType = arg.type.name + (arg.type.qtgType?"Object*":"");
            const QString objectInputType = (arg.type.qtgType?objectType + " ":inputType);

            if(type.qtgType)
            {
                resultTypes += QString("    m_%1(0),\n").arg(cammelCase);
                resultConstructorTypes += QString("    m_%1 = new %3Object(m_core.%1(), this);\n"
                                                  "    connect(m_%1.data(), &%3Object::coreChanged, this, &%4Object::core%2Changed);\n").arg(cammelCase, classCase, type.name, clssName);

                functions += preFnc + QString("void %1Object::set%2(%3 %4) {\n    if(m_%4 == %4) return;\n"
                                              "    if(m_%4) delete m_%4;\n    m_%4 = %4;\n"
                                              "    if(m_%4) {\n        m_%4->setParent(this);\n        m_core.set%2(m_%4->core());\n"
                                              "        connect(m_%4.data(), &%5Object::coreChanged, this, &%1Object::core%2Changed);\n    }\n"
                                              "    Q_EMIT %4Changed();\n    Q_EMIT coreChanged();\n}\n\n").arg(clssName, classCase, objectType, cammelCase, type.name);
                functions += preFnc + QString("%3 %1Object::%2() const {\n    return m_%2;\n}\n\n").arg(clssName, cammelCase, objectInputType);

                resultEqualOperator += QString("    m_%1->setCore(b.%1());\n").arg(cammelCase);

                resultPrivateSlots += preFnc + QString("void %1Object::core%2Changed() {\n    if(m_core.%3() == m_%3->core()) return;\n"
                        "    m_core.set%2(m_%3->core());\n    Q_EMIT %3Changed();\n    Q_EMIT coreChanged();\n}\n\n").arg(clssName, classCase, cammelCase);
            }
            else
            {
                functions += preFnc + QString("void %1Object::set%2(%3%4) {\n    if(m_core.%4() == %4) return;\n"
                                     "    m_core.set%2(%4);\n    Q_EMIT %4Changed();\n    Q_EMIT coreChanged();\n}\n\n").arg(clssName, classCase, inputType, cammelCase);
                functions += preFnc + QString("%3 %1Object::%2() const {\n    return m_core.%2();\n}\n\n").arg(clssName, cammelCase, type.name);
            }

            resultEqualOperatorEmits += QString("    Q_EMIT %1Changed();\n").arg(cammelCase);
        }
    }
    resultEqualOperatorEmits += "    Q_EMIT coreChanged();\n    return *this;\n";

    baseResult += preFnc + QString("%1Object::%1Object(const %1 &core, QObject *parent) :\n").arg(clssName);
    baseResult += QString("    TelegramTypeQObject(parent),\n") + resultTypes + QString("    m_core(core)\n");
    baseResult += "{\n" + resultConstructorTypes + "}\n\n";
    baseResult += preFnc + QString("%1Object::%1Object(QObject *parent) :\n").arg(clssName);
    baseResult += QString("    TelegramTypeQObject(parent),\n") + resultTypes + QString("    m_core()\n");
    baseResult += "{\n" + resultConstructorTypes + "}\n\n";
    baseResult += preFnc + QString("%1Object::~%1Object() {\n}\n\n").arg(clssName);
    baseResult += functions;
    baseResult += preFnc + QString("%1Object &%1Object::operator =(const %1 &b) {\n%2\n%3}\n\n").arg(clssName, resultEqualOperator, resultEqualOperatorEmits);
    baseResult += preFnc + QString("bool %1Object::operator ==(const %1 &b) const {\n%2}\n\n").arg(clssName, shiftSpace(resultEqualCheckOperator, 1));

    baseResult += preFnc + QString("void %1Object::setClassType(quint32 classType) {\n%2\n    if(m_core.classType() == result) return;\n"
                      "    m_core.setClassType(result);\n    Q_EMIT classTypeChanged();\n    Q_EMIT coreChanged();\n}\n\n").arg(clssName, typeSwitch);
    baseResult += preFnc + QString("quint32 %1Object::classType() const {\n%2\n    return result;\n}\n\n").arg(clssName, typeSwitchBack);

    baseResult += preFnc + QString("void %1Object::setCore(const %1 &core) {\n    operator =(core);\n}\n\n").arg(clssName);
    baseResult += preFnc + QString("%1 %1Object::core() const {\n    return m_core;\n}\n\n").arg(clssName);

    baseResult += resultPrivateSlots;

    if(!m_inlineMode)
        result += baseResult;

    QFile file(m_dst + "/" + clssName.toLower() + "object.cpp");
    if(file.open(QFile::WriteOnly))
    {
        file.write(GENERATOR_SIGNATURE("//"));
        file.write(result.toUtf8());
        file.close();
    }

    return baseResult;
}

void TypeObjectGenerator::writeType(const QString &name, const QList<GeneratorTypes::TypeStruct> &types)
{
    writeTypeHeader(name, types);
    if(!m_inlineMode)
        writeTypeClass(name, types);
}

void TypeObjectGenerator::writePri(const QStringList &types)
{
    QString result = "\n";
    QString headers = "HEADERS += \\\n    $$PWD/typeobjects.h \\\n    $$PWD/telegramtypeqobject.h \\\n    $$PWD/qmltools.h \\\n";
    QString sources = "SOURCES += \\\n    $$PWD/telegramtypeqobject.cpp \\\n";
    for(int i=0; i<types.count(); i++)
    {
        const QString &t = classCaseType(types[i]);
        const bool last = (i == types.count()-1);

        headers += QString(last? "    $$PWD/%1object.h\n" : "    $$PWD/%1object.h \\\n").arg(t.toLower());
        sources += QString(last? "    $$PWD/%1object.cpp\n" : "    $$PWD/%1object.cpp \\\n").arg(t.toLower());
    }

    result += headers + "\n";
    result += sources + "\n";
//    result = result.replace("$$PWD","telegram/types");

    QFile file(m_dst + "/typeobjects.pri");
    if(!file.open(QFile::WriteOnly))
        return;

    file.write(GENERATOR_SIGNATURE("#"));
    file.write(result.toUtf8());
    file.close();
}

void TypeObjectGenerator::writeQmlRegister(const QStringList &types)
{
    QString result = "\n#include \"typeobjects.h\"\n#include <qqml.h>\n\n";
    result += "void qtelegramRegisterQmlTypes(const char *uri, int major, int minor) {\n";
    result += QString("    qmlRegisterType<TelegramTypeQObject>(uri, major, minor, \"TelegramTypeQObject\");\n");
    for(int i=0; i<types.count(); i++)
    {
        const QString &t = classCaseType(types[i]);
        result += QString("    qmlRegisterType<%1Object>(uri, major, minor, \"%1\");\n").arg(t);
    }
    result += "}\n\n";

    QFile file(m_dst + "/qmltools.h");
    if(!file.open(QFile::WriteOnly))
        return;

    file.write(GENERATOR_SIGNATURE("//"));
    file.write(result.toUtf8());
    file.close();
}

void TypeObjectGenerator::writeMainHeader(const QStringList &types)
{
    QString result = "\n#include \"telegramtypeqobject.h\"\n";
    for(int i=0; i<types.count(); i++)
    {
        const QString &t = classCaseType(types[i]);
        result += QString("#include \"%1object.h\"\n").arg(t.toLower());
    }

    QFile file(m_dst + "/typeobjects.h");
    if(!file.open(QFile::WriteOnly))
        return;

    file.write(GENERATOR_SIGNATURE("//"));
    file.write(result.toUtf8());
    file.close();
}

void TypeObjectGenerator::copyEmbeds()
{
    const QString &fcpp = m_dst + "/telegramtypeqobject.cpp";
    QFile::remove(fcpp);
    QFile::copy(":/embeds/telegramtypeqobject.cpp", fcpp);
    QFile(fcpp).setPermissions(QFileDevice::ReadUser|QFileDevice::WriteUser|
                                  QFileDevice::ReadGroup|QFileDevice::WriteGroup);

    const QString &fh = m_dst + "/telegramtypeqobject.h";
    QFile::remove(fh);
    QFile::copy(":/embeds/telegramtypeqobject.h", fh);
    QFile(fh).setPermissions(QFileDevice::ReadUser|QFileDevice::WriteUser|
                                  QFileDevice::ReadGroup|QFileDevice::WriteGroup);
}

void TypeObjectGenerator::extract(const QString &data)
{
    QDir().mkpath(m_dst);

    QMap<QString, QList<GeneratorTypes::TypeStruct> > types = extractTypes(data, "object", QString(), "types");
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
    writeQmlRegister(types.keys());
    copyEmbeds();
}

