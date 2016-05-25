#ifndef DOCUMENTATTRIBUTELIST_H
#define DOCUMENTATTRIBUTELIST_H

#include <QObject>

#include "telegramqml_global.h"

class DocumentAttribute;
class DocumentAttributeObject;
class DocumentAttributeListPrivate;
class TELEGRAMQMLSHARED_EXPORT DocumentAttributeList : public QObject
{
    Q_OBJECT

    Q_PROPERTY(DocumentAttributeObject* first READ first NOTIFY firstChanged)
    Q_PROPERTY(DocumentAttributeObject* last READ last NOTIFY lastChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    DocumentAttributeList(QObject *parent = 0);
    DocumentAttributeList(const QList<DocumentAttribute> & another, QObject *parent = 0);
    ~DocumentAttributeList();

    void operator =( const QList<DocumentAttribute> & another );

    DocumentAttributeObject *first() const;
    DocumentAttributeObject *last() const;

    int count() const;

public Q_SLOTS:
    DocumentAttributeObject *at( int idx );

Q_SIGNALS:
    void firstChanged();
    void lastChanged();
    void countChanged();

private:
    DocumentAttributeListPrivate *p;
};

Q_DECLARE_METATYPE(DocumentAttributeList*)

#endif // DOCUMENTATTRIBUTELIST_H
