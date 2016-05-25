#ifndef TQBASEOBJECT_H
#define TQBASEOBJECT_H

#include <QObject>
#include <QStringList>

#define FUNCTION_NAME(FNC) #FNC

class TqBaseObject
{
public:
    TqBaseObject(){}
    virtual ~TqBaseObject(){}
};

#endif // TQBASEOBJECT_H
