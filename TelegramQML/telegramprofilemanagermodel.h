#ifndef TELEGRAMPROFILEMANAGERMODEL_H
#define TELEGRAMPROFILEMANAGERMODEL_H

#include "telegramabstractlistmodel.h"
#include "telegramqml_global.h"

#include <QStringList>
#include <QQmlComponent>

class TelegramEngine;
class TelegramProfileManagerModelPrivate;
class TELEGRAMQMLSHARED_EXPORT TelegramProfileManagerModel : public TelegramAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(DataRole)
    Q_ENUMS(AddResult)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QQmlComponent* engineDelegate READ engineDelegate WRITE setEngineDelegate NOTIFY engineDelegateChanged)
    Q_PROPERTY(bool initializing READ initializing NOTIFY initializingChanged)

public:
    enum DataRole {
        DataPhoneNumber = Qt::UserRole,
        DataMute,
        DataEngine
    };

    enum AddResult {
        AddResultSucessfully,
        AddResultExists,
        AddResultInvalidPhone,
        AddResultDatabaseError
    };

    TelegramProfileManagerModel(QObject *parent = 0);
    ~TelegramProfileManagerModel();

    class TelegramProfileManagerModelItem id(const QModelIndex &index) const;
    int count() const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    void setSource(const QString &source);
    QString source() const;

    void setEngineDelegate(QQmlComponent *component);
    QQmlComponent *engineDelegate() const;

    bool initializing() const;
    static QStringList requiredProperties();

public Q_SLOTS:
    void addNew();
    int add(const QString &phoneNumber, bool mute, TelegramEngine *engine);
    bool remove(const QString &phoneNumber);

Q_SIGNALS:
    void sourceChanged();
    void engineDelegateChanged();
    void initializingChanged();

private:
    void changed(const QList<class TelegramProfileManagerModelItem> &list);
    void init();
    void initTables();
    void initBuffer();

protected:
    void itemsChanged_slt();
    void setInitializing(bool initializing);

private:
    TelegramProfileManagerModelPrivate *p;
};

#endif // TELEGRAMPROFILEMANAGERMODEL_H
