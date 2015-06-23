#ifndef CONTACTFOUND_H
#define CONTACTFOUND_H

#include <QtGlobal>

class ContactFound
{
public:

    enum ContactFoundType {
       typeContactFound = 0xea879f95
    };

    ContactFound() :
        m_userId(0),
        m_classType(typeContactFound) {}

    void setUserId(qint32 userId) {
        m_userId = userId;
    }
    qint32 userId() const {
        return m_userId;
    }
    void setClassType(ContactFoundType classType) {
        m_classType = classType;
    }
    ContactFoundType classType() const {
        return m_classType;
    }

private:
    qint32 m_userId;
    ContactFoundType m_classType;
};

#endif // CONTACTFOUND_H
