#ifndef TGABSTRACTLISTMODEL_H
#define TGABSTRACTLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>

#include "telegramqml_global.h"

class TELEGRAMQMLSHARED_EXPORT TgAbstractListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    TgAbstractListModel(QObject *parent = 0);
    virtual ~TgAbstractListModel();

    Q_INVOKABLE QStringList roles() const;

public Q_SLOTS:
    QVariant get(int index, int role) const;
    QVariantMap get(int index) const;
};

#endif // TGABSTRACTLISTMODEL_H
