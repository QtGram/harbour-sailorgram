#ifndef CONTACTLINK
#define CONTACTLINK

#include <QtGlobal>

class ContactLink
{
public:

    enum ContactLinkType {
        typeContactLinkUnknown = 0x5f4f9247,
        typeContactLinkNone = 0xfeedd3ad,
        typeContactLinkHasPhone = 0x268f3f59,
        typeContactLinkContact = 0xd502c2d0,
    };

    ContactLink(ContactLinkType type) :
        m_classType(type) {}
    ~ContactLink() {}

    void setClassType(ContactLinkType classType) {
        m_classType = classType;
    }
    ContactLinkType classType() const {
        return m_classType;
    }

private:
    ContactLinkType m_classType;
};

#endif // CONTACTLINK

