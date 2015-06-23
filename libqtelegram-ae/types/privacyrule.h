#ifndef PRIVACYRULE
#define PRIVACYRULE

#include <QtGlobal>
#include <QList>

class PrivacyRule
{
public:

    enum PrivacyRuleType {
       typePrivacyValueAllowContacts = 0xfffe1bac,
       typePrivacyValueAllowAll = 0x65427b82,
       typePrivacyValueAllowUsers = 0x4d5bbe0c,
       typePrivacyValueDisallowContacts = 0xf888fa1a,
       typePrivacyValueDisallowAll = 0x8b73e763,
       typePrivacyValueDisallowUsers = 0xc7f49b7
    };

    PrivacyRule(PrivacyRuleType classType) :
        m_classType(classType) {}

    void setUsers(const QList<int> &users) {
        m_users = users;
    }
    QList<int> users() {
        return m_users;
    }
    void setClassType(PrivacyRuleType classType) {
        m_classType = classType;
    }
    PrivacyRuleType classType() const {
        return m_classType;
    }

private:
    QList<int> m_users;
    PrivacyRuleType m_classType;
};

#endif // PRIVACYRULE

