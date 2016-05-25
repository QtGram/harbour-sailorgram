#include "typegenerator.h"
#include "lqtgtypegenerator.h"
#include "functiongenerator.h"
#include "coretypesgenerator.h"
#include "typeobjectgenerator.h"
#include "apigenerator.h"
#include "telegramcoregenerator.h"
#include "documentsgenerator.h"
#include "databasegenerator.h"

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    if(app.arguments().count() < 4)
    {
        qDebug() << "Bad input!\n"
                    "Usage: libqtelegram-code-generator layer schema-path output-dir\n"
                    "Example: libqtelegram-code-generator 29 /path/to/schema.txt /path/to/out/dir";
        return 0;
    }

    const QString layerVersion = app.arguments().at(1);
    const QString schemaPath = app.arguments().at(2);
    const QString destPath = app.arguments().at(3);

    QFile file(schemaPath);
    if(!file.open(QFile::ReadOnly))
        return -1;

    QDir().mkpath(destPath);

    const QString &data = file.readAll();

    TypeGenerator(destPath + "/telegram/types", true).extract(data);
    LqtgTypeGenerator(destPath + "/telegram/customtypes", true).extract(data);
    FunctionGenerator(destPath + "/telegram/functions", true).extract(data);
    TypeObjectGenerator(destPath + "/telegram/objects", true).extract(data);
    CoreTypesGenerator(destPath + "/telegram/").extract(data, layerVersion);
    ApiGenerator(destPath + "/telegram/").extract(data);
    TelegramCoreGenerator(destPath + "/telegram/").extract(data);
    DocumentsGenerator(destPath + "/telegram/documents/").extract(data);
//    DatabaseGenerator(destPath + "/telegram/").extract(data);

    return 0;
}
