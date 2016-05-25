#include "apigenerator.h"

#include <QFile>
#include <QDebug>

ApiGenerator::ApiGenerator(const QString &dst) :
    AbstractGenerator(),
    m_dst(dst)
{
}

ApiGenerator::~ApiGenerator()
{
}

void ApiGenerator::extract(const QString &data)
{
    QMap<QString, QList<GeneratorTypes::FunctionStruct> > functions = extractFunctions(data);
    writeHeader(functions);
    writeCpp(functions);
}

void ApiGenerator::writeHeader(const QMap<QString, QList<GeneratorTypes::FunctionStruct> > &functions)
{
    QFile file(":/embeds/telegramapi.h");
    file.open(QFile::ReadOnly);

    QString headerOrgData = file.readAll();

    QString result = headerOrgData;
    QString methodsResult;
    QString signalsResult;
    QString errorsResult;
    QString privateResult;
    QString queriesResult;
    QString answersResult;
    QString answerErrorsResult;

    QMapIterator<QString, QList<GeneratorTypes::FunctionStruct> > i(functions);
    while(i.hasNext())
    {
        i.next();
        if(!methodsResult.isEmpty())
            methodsResult += "\n";
        if(!signalsResult.isEmpty())
            signalsResult += "\n";
        if(!queriesResult.isEmpty())
            queriesResult += "\n";
        if(!answersResult.isEmpty())
            answersResult += "\n";
        if(!answerErrorsResult.isEmpty())
            answerErrorsResult += "\n";
        if(!errorsResult.isEmpty())
            errorsResult += "\n";

        const QString &name = i.key();
        const QList<GeneratorTypes::FunctionStruct> &types = i.value();
        foreach(const GeneratorTypes::FunctionStruct &t, types)
        {
            if(!methodsResult.isEmpty())
                methodsResult += "\n";
            if(!signalsResult.isEmpty())
                signalsResult += "\n";
            if(!queriesResult.isEmpty())
                queriesResult += "\n";
            if(!answersResult.isEmpty())
                answersResult += "\n";
            if(!answerErrorsResult.isEmpty())
                answerErrorsResult += "\n";
            if(!errorsResult.isEmpty())
                errorsResult += "\n";

            QString functionName = name.toLower() + t.functionName[0].toUpper() + t.functionName.mid(1);
            QString arguments;

            const QList<GeneratorTypes::ArgStruct> &args = t.type.args;
            foreach(const GeneratorTypes::ArgStruct &arg, args)
            {
                if(arg.isFlag)
                    continue;

                if(arg.type.constRefrence)
                    arguments += QString("const %1 &%2").arg(arg.type.name, arg.argName);
                else
                    arguments += QString("%1 %2").arg(arg.type.name, arg.argName);
                arguments += ", ";
            }

            QString returnArg;
            if(t.returnType.constRefrence)
                returnArg = QString("const %1 &result").arg(t.returnType.name);
            else
                returnArg = QString("%1 result").arg(t.returnType.name);

            methodsResult += QString("qint64 %1(%2const QVariant &attachedData = QVariant(), Session *session = 0);").arg(functionName, arguments);
            signalsResult += QString("void %1Answer(qint64 msgId, %2, const QVariant &attachedData);").arg(functionName, returnArg);
            errorsResult += QString("void %1Error(qint64 msgId, qint32 errorCode, const QString &errorText, const QVariant &attachedData);").arg(functionName);
            queriesResult += QString("QueryMethods %1Methods;").arg(functionName);
            answersResult += QString("void on%1Answer(Query *q, InboundPkt &inboundPkt);").arg(classCaseType(functionName));
            answerErrorsResult += QString("void on%1Error(Query *q, qint32 errorCode, const QString &errorText);").arg(classCaseType(functionName));
        }
    }

    privateResult += queriesResult + "\n\n\n";
    privateResult += answersResult + "\n\n\n";
    privateResult += answerErrorsResult;

    result.replace("/*! === methods === !*/", shiftSpace(methodsResult, 1));
    result.replace("/*! === signals === !*/", shiftSpace(signalsResult, 1) + "\n\n" + shiftSpace(errorsResult, 1));
    result.replace("/*! === privates === !*/", shiftSpace(privateResult, 1));

    QFile outFile(m_dst + "/telegramapi.h");
    outFile.open(QFile::WriteOnly);
    outFile.write(result.toUtf8());
    outFile.close();
}

void ApiGenerator::writeCpp(const QMap<QString, QList<GeneratorTypes::FunctionStruct> > &functions)
{
    QFile file(":/embeds/telegramapi.cpp");
    file.open(QFile::ReadOnly);

    QString headerOrgData = file.readAll();

    QString result = headerOrgData;
    QString constructorsResult;
    QString methodsResult;

    QMapIterator<QString, QList<GeneratorTypes::FunctionStruct> > i(functions);
    while(i.hasNext())
    {
        i.next();
        if(!constructorsResult.isEmpty())
            constructorsResult += "\n";
        if(!methodsResult.isEmpty())
            methodsResult += "\n";

        const QString &name = i.key();
        const QList<GeneratorTypes::FunctionStruct> &types = i.value();
        foreach(const GeneratorTypes::FunctionStruct &t, types)
        {
            if(!constructorsResult.isEmpty())
                constructorsResult += "\n";
            if(!methodsResult.isEmpty())
                methodsResult += "\n";

            QString functionName = name.toLower() + t.functionName[0].toUpper() + t.functionName.mid(1);
            QString classCase = classCaseType(functionName);
            QString arguments;
            QString argumentNames;

            const QList<GeneratorTypes::ArgStruct> &args = t.type.args;
            foreach(const GeneratorTypes::ArgStruct &arg, args)
            {
                if(arg.isFlag)
                    continue;

                argumentNames += ", ";

                argumentNames += arg.argName;
                if(arg.type.constRefrence)
                    arguments += QString("const %1 &%2").arg(arg.type.name, arg.argName);
                else
                    arguments += QString("%1 %2").arg(arg.type.name, arg.argName);

                arguments += ", ";
            }

            QString returnArg;
            if(t.returnType.constRefrence)
                returnArg = QString("const %1 &result").arg(t.returnType.name);
            else
                returnArg = QString("const %1 result").arg(t.returnType.name);

            QString methodInner = QString("if(!session) session = mMainSession;\n"
                                          "CHECK_SESSION(session)\n"
                                          "DEBUG_FUNCTION\n"
                                          "OutboundPkt p(mSettings);\n"
                                          "INIT_MAIN_CONNECTION(session)\n"
                                          "Functions::%1::%2(&p%3);\n"
                                          "return session->sendQuery(p, &%4Methods, attachedData, \"%1->%2\" );").arg(name)
                                  .arg(t.functionName).arg(argumentNames).arg(functionName);

            QString answersInnter = QString("%1 = Functions::%2::%3Result(&inboundPkt);\n" +
                                            QString(t.returnType.qtgType?
                                            QString("if(result.error())\n    on%1Error(q, -1, \"LIBQTELEGRAM_INTERNAL_ERROR\");\n"
                                            "else\n    ").arg(classCase):QString()) +
                                            "Q_EMIT %4Answer(q->mainMsgId(), result, q->extra());")
                                  .arg(returnArg, name, t.functionName, functionName);

            QString errorsInnter = QString("bool accepted = false;\n"
                                           "onError(q, errorCode, errorText, q->extra(), accepted);\n"
                                           "if(!accepted)\n"
                                           "    Q_EMIT %1Error(q->mainMsgId(), errorCode, errorText, q->extra());\n").arg(functionName);

            constructorsResult += QString("%1Methods.onAnswer = &TelegramApi::on%2Answer;\n"
                                          "%1Methods.onError = &TelegramApi::on%2Error;").arg(functionName, classCase);
            methodsResult += QString("qint64 TelegramApi::%1(%2const QVariant &attachedData, Session *session) {\n%3}\n\n").arg(functionName, arguments, shiftSpace(methodInner, 1))
                           + QString("void TelegramApi::on%1Answer(Query *q, InboundPkt &inboundPkt) {\n%2}\n\n").arg(classCase, shiftSpace(answersInnter, 1))
                           + QString("void TelegramApi::on%1Error(Query *q, qint32 errorCode, const QString &errorText) {\n%2}\n").arg(classCase, shiftSpace(errorsInnter, 1));
        }
    }

    result.replace("/*! === constructor === !*/", shiftSpace(constructorsResult, 1));
    result.replace("/*! === methods === !*/", methodsResult);

    QFile outFile(m_dst + "/telegramapi.cpp");
    outFile.open(QFile::WriteOnly);
    outFile.write(result.toUtf8());
    outFile.close();
}

