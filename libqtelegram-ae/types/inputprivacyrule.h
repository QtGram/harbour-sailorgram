#ifndef INPUTPRIVACYRULE
#define INPUTPRIVACYRULE

#include <QtGlobal>
#include <QList>

class InputPrivacyRule
{
public:

    enum InputPrivacyRuleType {
       typeInputPrivacyValueAllowContacts = 0xfffe1bac,
       typeInputPrivacyValueAllowAll = 0x65427b82,
       typeInputPrivacyValueAllowUsers = 0x4d5bbe0c,
       typeInputPrivacyValueDisallowContacts = 0xf888fa1a,
       typeInputPrivacyValueDisallowAll = 0x8b73e763,
       typeInputPrivacyValueDisallowUsers = 0xc7f49b7
    };

    InputPrivacyRule(InputPrivacyRuleType classType) :
        m_classType(classType) {}

    void setUsers(const QList<qint32> &users) {
        m_users = users;
    }
    QList<qint32> users() const {
        return m_users;
    }
    void setClassType(InputPrivacyRuleType classType) {
        m_classType = classType;
    }
    InputPrivacyRuleType classType() const {
        return m_classType;
    }

private:
    QList<qint32> m_users;
    InputPrivacyRuleType m_classType;
};

#endif // INPUTPRIVACYRULE

