#include "translationinfoitem.h"

#include <QJsonObject>
#include <QLocale>

TranslationInfoItem::TranslationInfoItem(QObject *parent) :
    QObject(parent)
{

}

TranslationInfoItem::TranslationInfoItem(const QJsonObject &jsonobject, QObject *parent) :
    QObject(parent),
    _coordinators(jsonobject.value("coordinators").toVariant().toStringList()),
    _translators(jsonobject.value("translators").toVariant().toStringList()),
    _reviewers(jsonobject.value("reviewers").toVariant().toStringList())
{
    QString trcode = jsonobject.value("language_code").toString();
    QLocale trlocale(trcode);
    _name = trlocale.nativeLanguageName();
    _name[0] = _name[0].toUpper();
    if (trcode.length() > 2)
        _name += " (" + trlocale.nativeCountryName() + ")";
}

QString TranslationInfoItem::name() const
{
    return _name;
}

QStringList TranslationInfoItem::coordinators() const
{
    return _coordinators;
}

QStringList TranslationInfoItem::translators() const
{
    return _translators;
}

QStringList TranslationInfoItem::reviewers() const
{
    return _reviewers;
}
