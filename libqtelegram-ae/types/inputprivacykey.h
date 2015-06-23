#ifndef INPUTPRIVACYKEY
#define INPUTPRIVACYKEY

#include <QtGlobal>

class InputPrivacyKey
{
public:
    enum InputPrivacyKeyType {
       typeInputPrivacyKeyStatusTimestamp = 0xee3b272a
    };

    InputPrivacyKey(InputPrivacyKeyType classType) :
        m_classType(classType) {}

    void setClassType(InputPrivacyKeyType classType) {
        m_classType = classType;
    }
    InputPrivacyKeyType classType() const {
        return m_classType;
    }

private:
    InputPrivacyKeyType m_classType;
};

#endif // INPUTPRIVACYKEY

