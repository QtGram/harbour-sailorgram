#ifndef CONFIG
#define CONFIG

#include <QList>

#include "dcoption.h"
#include "disabledfeature.h"

class Config
{
public:

    enum ConfigType {
        typeConfig = 0x3e6f732a
    };

    Config() :
        m_testMode(false),
        m_classType(typeConfig),
        m_date(0),
        m_expires(0),
        m_thisDc(0),
        m_chatSizeMax(0),
        m_broadcastSizeMax(0),
        m_onlineUpdatePeriodMs(0),
        m_offlineBlurTimeoutMs(0),
        m_offlineIdleTimeoutMs(0),
        m_onlineCloudTimeoutMs(0),
        m_notifyCloudDelayMs(0),
        m_notifyDefaultDelayMs(0),
        m_chatBigSize(0) {}
    ~Config() {}

    qint32 date() const {
        return m_date;
    }
    void setDate(const qint32 &date) {
        m_date = date;
    }
    qint32 expires() const {
        return m_expires;
    }
    void setExpires(const qint32 &expires) {
        m_expires = expires;
    }
    bool testMode() const {
        return m_testMode;
    }
    void setTestMode(bool testMode) {
        m_testMode = testMode;
    }
    qint32 thisDc() const {
        return m_thisDc;
    }
    void setThisDc(const qint32 &thisDc) {
        m_thisDc = thisDc;
    }
    QList<DcOption> dcOptions() const {
        return m_dcOptions;
    }
    void setDcOptions(const QList<DcOption> &dcOptions) {
        m_dcOptions = dcOptions;
    }
    qint32 chatSizeMax() const {
        return m_chatSizeMax;
    }
    void setChatSizeMax(const qint32 &chatSizeMax) {
        m_chatSizeMax = chatSizeMax;
    }
    qint32 broadcastSizeMax() const {
        return m_broadcastSizeMax;
    }
    void setBroadcastSizeMax(const qint32 &broadcastSizeMax) {
        m_broadcastSizeMax = broadcastSizeMax;
    }
    qint32 onlineUpdatePeriodMs() const {
        return m_onlineUpdatePeriodMs;
    }
    void setOnlineUpdatePeriodMs(const qint32 &onlineUpdatePeriodMs) {
        m_onlineUpdatePeriodMs = onlineUpdatePeriodMs;
    }
    qint32 offlineBlurTimeoutMs() const {
        return m_offlineBlurTimeoutMs;
    }
    void setOfflineBlurTimeoutMs(const qint32 &offlineBlurTimeoutMs) {
        m_offlineBlurTimeoutMs = offlineBlurTimeoutMs;
    }
    qint32 offlineIdleTimeoutMs() const {
        return m_offlineIdleTimeoutMs;
    }
    void setOfflineIdleTimeoutMs(const qint32 &offlineIdleTimeoutMs) {
        m_offlineIdleTimeoutMs = offlineIdleTimeoutMs;
    }
    qint32 onlineCloudTimeoutMs() const {
        return m_onlineCloudTimeoutMs;
    }
    void setOnlineCloudTimeoutMs(const qint32 &onlineCloudTimeoutMs) {
        m_onlineCloudTimeoutMs = onlineCloudTimeoutMs;
    }
    qint32 notifyCloudDelayMs() const {
        return m_notifyCloudDelayMs;
    }
    void setNotifyCloudDelayMs(const qint32 &notifyCloudDelayMs) {
        m_notifyCloudDelayMs = notifyCloudDelayMs;
    }
    qint32 notifyDefaultDelayMs() const {
        return m_notifyDefaultDelayMs;
    }
    void setNotifyDefaultDelayMs(const qint32 &notifyDefaultDelayMs) {
        m_notifyDefaultDelayMs = notifyDefaultDelayMs;
    }
    qint32 chatBigSize() const {
        return m_chatBigSize;
    }
    void setChatBigSize(const qint32 &chatBigSize) {
        m_chatBigSize = chatBigSize;
    }
    QList<DisabledFeature> disabledFeatures() const {
        return m_disabledFeatures;
    }
    void setDisabledFeatures(const QList<DisabledFeature> &disabledFeatures) {
        m_disabledFeatures = disabledFeatures;
    }
    void setClassType(ConfigType classType) {
        m_classType = classType;
    }
    ConfigType classType() const {
        return m_classType;
    }

private:
    QList<DcOption> m_dcOptions;
    QList<DisabledFeature> m_disabledFeatures;
    bool m_testMode;
    ConfigType m_classType;
    qint32 m_date;
    qint32 m_expires;
    qint32 m_thisDc;
    qint32 m_chatSizeMax;
    qint32 m_broadcastSizeMax;
    qint32 m_onlineUpdatePeriodMs;
    qint32 m_offlineBlurTimeoutMs;
    qint32 m_offlineIdleTimeoutMs;
    qint32 m_onlineCloudTimeoutMs;
    qint32 m_notifyCloudDelayMs;
    qint32 m_notifyDefaultDelayMs;
    qint32 m_chatBigSize;
};

#endif // CONFIG
