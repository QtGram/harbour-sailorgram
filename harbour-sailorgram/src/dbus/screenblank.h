#ifndef SCREENBLANK_H
#define SCREENBLANK_H

#include <QObject>
#include <QDBusMessage>
#include <QDBusConnection>

class ScreenBlank : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

    public:
        explicit ScreenBlank(QObject *parent = 0);
        bool enabled() const;
        void setEnabled(bool e);

    signals:
        void enabledChanged();

    private:
        bool _enabled;
};

#endif // SCREENBLANK_H
