#include "documentattributelist.h"
#include "objects/types.h"

class DocumentAttributeListPrivate
{
public:
    QList<DocumentAttributeObject*> list;
};

DocumentAttributeList::DocumentAttributeList(QObject *parent) :
    QObject(parent)
{
    p = new DocumentAttributeListPrivate;
}

DocumentAttributeList::DocumentAttributeList(const QList<DocumentAttribute> & another, QObject *parent) :
    QObject(parent)
{
    p = new DocumentAttributeListPrivate;
    operator =(another);
}

void DocumentAttributeList::operator =(const QList<DocumentAttribute> &another)
{
    while(!p->list.isEmpty())
        p->list.takeFirst()->deleteLater();

    Q_FOREACH( const DocumentAttribute & size, another )
    {
        DocumentAttributeObject *obj = new DocumentAttributeObject(size, this);
        p->list << obj;
    }

    Q_EMIT firstChanged();
    Q_EMIT lastChanged();
    Q_EMIT countChanged();
}

DocumentAttributeObject *DocumentAttributeList::first() const
{
    if( p->list.isEmpty() )
        return 0;

    return p->list.first();
}

DocumentAttributeObject *DocumentAttributeList::last() const
{
    if( p->list.isEmpty() )
        return 0;

    return p->list.last();
}

int DocumentAttributeList::count() const
{
    return p->list.count();
}

DocumentAttributeObject *DocumentAttributeList::at(int idx)
{
    return p->list.at(idx);
}

DocumentAttributeList::~DocumentAttributeList()
{
    delete p;
}
