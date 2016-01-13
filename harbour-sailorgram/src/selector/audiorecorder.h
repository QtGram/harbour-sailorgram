#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <QObject>
#include <QMediaRecorder>
#include <QAudioRecorder>
#include <QUrl>

class AudioRecorder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int size READ size NOTIFY sizeChanged)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)

    public:
        explicit AudioRecorder(QObject* parent = 0);
        int size() const;
        int duration() const;
        QString statusMessage() const;

    public slots:
        void start();
        void stop();

    signals:
        void sizeChanged();
        void durationChanged();
        void statusMessageChanged();

    protected slots:
        void updateState();

    private:
        QString _outputfile;
        QString _statusmessage;
        QAudioRecorder* _audiorecorder;
};

#endif // AUDIORECORDER_H
