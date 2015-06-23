#ifndef ACCOUNTDAYSTTL
#define ACCOUNTDAYSTTL

#include <QtGlobal>

class AccountDaysTTL
{
public:

    enum AccountDaysTTLType {
       typeAccountDaysTTLType = 0xb8d0afdf
    };

    AccountDaysTTL() :
        m_days(0),
        m_classType(typeAccountDaysTTLType) {}

    void setDays(qint32 days) {
        m_days = days;
    }
    qint32 days() const {
        return m_days;
    }
    void setClassType(AccountDaysTTLType classType) {
        m_classType = classType;
    }
    AccountDaysTTLType classType() const {
        return m_classType;
    }

private:
    qint32 m_days;
    AccountDaysTTLType m_classType;
};

#endif // ACCOUNTDAYSTTL

