#ifndef PRIVACYKEY
#define PRIVACYKEY

#include <QtGlobal>

class PrivacyKey
{
public:

    enum PrivacyKeyType {
       typePrivacyKeyStatusTimestamp = 0xbc2eab30
    };

    PrivacyKey(PrivacyKeyType classType) :
        m_classType(classType) {}

    void setClassType(PrivacyKeyType classType) {
        m_classType = classType;
    }
    PrivacyKeyType classType() const {
        return m_classType;
    }

private:
    PrivacyKeyType m_classType;
};

#endif // PRIVACYKEY

