#ifndef DISABLEDFEATURE
#define DISABLEDFEATURE

#include <QtGlobal>

class DisabledFeature
{
public:

    enum DisabledFeatureType {
       typeDisabledFeature = 0xae636f24
    };

    DisabledFeature() :
        m_classType(typeDisabledFeature) {}

    void setFeature(const QString &feature) {
        m_feature = feature;
    }
    QString feature() const {
        return m_feature;
    }
    void setDescription(const QString &description) {
        m_description = description;
    }
    QString description() const {
        return m_description;
    }
    void setClassType(DisabledFeatureType classType) {
        m_classType = classType;
    }
    DisabledFeatureType classType() const {
        return m_classType;
    }

private:
    QString m_feature;
    QString m_description;
    DisabledFeatureType m_classType;
};

#endif // DISABLEDFEATURE

