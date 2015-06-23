#ifndef ACCOUNTPASSWORD
#define ACCOUNTPASSWORD

#include <QByteArray>

class AccountPassword
{
public:
    enum AccountPasswordType {
        typeAccountNoPassword = 0x5770e7a9,
        typeAccountPassword = 0x739e5f72
    };

    AccountPassword(AccountPasswordType type) :
        m_classType(type) {}
    virtual ~AccountPassword() {}

    QByteArray newSalt() const {
        return m_newSalt;
    }
    void setNewSalt(const QByteArray &newSalt) {
        m_newSalt = newSalt;
    }
    QByteArray currentSalt() const {
        return m_currentSalt;
    }
    void setCurrentSalt(const QByteArray &currentSalt) {
        m_currentSalt = currentSalt;
    }
    QString hint() const {
        return m_hint;
    }
    void setHint(const QString &hint) {
        m_hint = hint;
    }
    void setClassType(AccountPasswordType classType) {
        m_classType = classType;
    }
    AccountPasswordType classType() const {
        return m_classType;
    }

private:
    QByteArray m_newSalt;
    QByteArray m_currentSalt;
    QString m_hint;
    AccountPasswordType m_classType;
};

#endif // ACCOUNTPASSWORD
