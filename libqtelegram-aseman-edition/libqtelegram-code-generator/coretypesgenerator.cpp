#include "coretypesgenerator.h"

#include <QStringList>
#include <QRegExp>
#include <QMap>
#include <QFile>
#include <QDir>
#include <QDebug>

CoreTypesGenerator::CoreTypesGenerator(const QString &dest) :
    m_dst(dest)
{

}

CoreTypesGenerator::~CoreTypesGenerator()
{

}

void CoreTypesGenerator::extract(const QString &data, const QString &layerVersion)
{
    QDir().mkpath(m_dst);

    QMap<QString,QString> types;
    const QStringList &lines = QString(data).split("\n",QString::SkipEmptyParts);
    bool hasAccess = false;
    foreach(const QString &line, lines)
    {
        const QString &l = line.trimmed();
        if(l.left(3) == "---")
        {
            if(l == "---types---")
                hasAccess = false;
            else
                hasAccess = true;

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
        if(name.contains("."))
            continue;

        types[name] = code;
    }

    write(types, layerVersion);
    writePri();
}

void CoreTypesGenerator::write(const QMap<QString, QString> &map, const QString &layerVersion)
{
    QString result = "#ifndef CORETYPES_H\n#define CORETYPES_H\n\n";
    result += "class CoreTypes \n{\npublic:\n    enum Types {\n";

    QString typesResult;
    QMapIterator<QString,QString> i(map);
    while(i.hasNext())
    {
        i.next();
        typesResult += QString("\ntype%1 = 0x%2,").arg(classCaseType(i.key()), i.value());
    }
    typesResult += "\ntypeLayerVersion = " + layerVersion;

    result += shiftSpace(typesResult, 2) + "    };\n};\n\n#endif //CORETYPES_H\n";

    QFile file(m_dst + "/coretypes.h");
    if(!file.open(QFile::WriteOnly))
        return;

    file.write(GENERATOR_SIGNATURE("//"));
    file.write(result.toUtf8());
    file.close();
}

void CoreTypesGenerator::writePri()
{
    QString result = "\nCONFIG += c++11\n\ninclude(functions/functions.pri)"
                     "\ninclude(types/types.pri)\ninclude(customtypes/customtypes.pri)\n"
                     "\ncontains(CONFIG, typeobjects) {\n"
                     "    include(objects/typeobjects.pri)\n}\n\n";

    QString headers = "HEADERS += \\\n    $$PWD/coretypes.h \\\n    $$PWD/telegramapi.h \\\n    "
                      "$$PWD/telegramcore.h \\\n    $$PWD/telegramcore_globals.h";
    QString sources = "SOURCES += \\\n    $$PWD/telegramapi.cpp \\\n    $$PWD/telegramcore.cpp";

    result += headers + "\n\n" + sources + "\n";
//    result = result.replace("$$PWD","telegram");

    QFile file(m_dst + "/telegram.pri");
    if(!file.open(QFile::WriteOnly))
        return;

    file.write(GENERATOR_SIGNATURE("#"));
    file.write(result.toUtf8());
    file.close();
}

