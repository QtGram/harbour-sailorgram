#ifndef TRANSLATIONINFOITEM_H
#define TRANSLATIONINFOITEM_H

#include <QObject>
#include <QStringList>

class QJsonObject;

class TranslationInfoItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT FINAL)
    Q_PROPERTY(QStringList coordinators READ coordinators CONSTANT FINAL)
    Q_PROPERTY(QStringList translators READ translators CONSTANT FINAL)
    Q_PROPERTY(QStringList reviewers READ reviewers CONSTANT FINAL)

public:
    explicit TranslationInfoItem(QObject *parent = 0);
    TranslationInfoItem(const QJsonObject &jsonobject, QObject *parent = 0);

    QString name() const;
    QStringList coordinators() const;
    QStringList translators() const;
    QStringList reviewers() const;

private:
    QString _name;
    QStringList _coordinators;
    QStringList _translators;
    QStringList _reviewers;
};

#endif // TRANSLATIONINFOITEM_H
