#ifndef AFFECTEDMESSAGES
#define AFFECTEDMESSAGES

#include "peernotifysettings.h"
#include "peer.h"

class AffectedMessages
{
public:

    enum AffectedMessagesType {
       typeAffectedMessages = 0x84d19185
    };

    AffectedMessages() :
        mPts(0),
        mPtsCount(0),
        m_classType(typeAffectedMessages) {}
    virtual ~AffectedMessages() {}

    qint32 pts() const {
        return mPts;
    }
    void setPts(const qint32 &pts) {
        mPts = pts;
    }
    qint32 ptsCount() const {
        return mPtsCount;
    }
    void setPtsCount(const qint32 &ptsCount) {
        mPtsCount = ptsCount;
    }
    void setClassType(AffectedMessagesType classType) {
        m_classType = classType;
    }
    AffectedMessagesType classType() const {
        return m_classType;
    }

private:
    qint32 mPts;
    qint32 mPtsCount;
    AffectedMessagesType m_classType;
};

#endif // AFFECTEDMESSAGES
