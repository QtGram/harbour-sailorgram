#include "audiorecorder.h"
#include <QStandardPaths>
#include <QStringBuilder>
#include <QDebug>
#include <QFileInfo>

AudioRecorder::AudioRecorder(QObject * parent) : QObject(parent)
{
    this->_audiorecorder = new QAudioRecorder(this);
    this->_outputfile = QStandardPaths::writableLocation(QStandardPaths::CacheLocation)  % "/voice-rec.ogg";
    QFile::remove(this->_outputfile);

    qWarning() << "Supported codecs" << this->_audiorecorder->supportedAudioCodecs();
    qWarning() << "Supported containers" << this->_audiorecorder->supportedContainers();
    qWarning() << "Supported samplerates" << this->_audiorecorder->supportedAudioSampleRates();

    connect(this->_audiorecorder, &QAudioRecorder::durationChanged, this, &AudioRecorder::durationChanged);
    connect(this->_audiorecorder, &QAudioRecorder::durationChanged, this, &AudioRecorder::sizeChanged);
    connect(this->_audiorecorder, &QAudioRecorder::stateChanged, this, &AudioRecorder::updateState);
    connect(this->_audiorecorder, &QAudioRecorder::stateChanged, this, &AudioRecorder::sizeChanged);
    //connect(m_audioRecorder, &QAudioRecorder::error, this, &AudioRecorder::updateState);

    this->updateState();
}

int AudioRecorder::size() const
{
    QFileInfo info(this->_outputfile);
    info.refresh();

    return info.size();
}

int AudioRecorder::duration() const
{
    return this->_audiorecorder->duration();
}

QString AudioRecorder::statusMessage() const
{
    return this->_statusmessage;
}

void AudioRecorder::start()
{
    if(this->_audiorecorder->state() != QMediaRecorder::StoppedState)
        return;

    this->_audiorecorder->setAudioInput(this->_audiorecorder->defaultAudioInput());

    QAudioEncoderSettings settings;
    settings.setCodec("audio/vorbis");
    settings.setChannelCount(1);
    settings.setSampleRate(48000);
    //settings.setBitRate(96000); // NOTE : MP3 only
    settings.setEncodingMode(QMultimedia::ConstantBitRateEncoding);

    this->_audiorecorder->setEncodingSettings(settings, QVideoEncoderSettings(), "ogg");
    this->_audiorecorder->setOutputLocation(QUrl(this->_outputfile));
    this->_audiorecorder->record();
}

void AudioRecorder::stop()
{
    if(this->_audiorecorder->state() != QMediaRecorder::RecordingState)
        return;

    this-> _audiorecorder->stop();
}

void AudioRecorder::updateState()
{
    if(this->_audiorecorder->error() != QMediaRecorder::NoError)
    {
        this->_statusmessage = tr("Error : %1").arg(this->_audiorecorder->errorString());
        emit statusMessageChanged();
        return;
    }

    switch(this->_audiorecorder->state())
    {
        case QMediaRecorder::RecordingState:
            this->_statusmessage = tr("Recording...");
            break;

        case QMediaRecorder::StoppedState:
            this->_statusmessage = tr("Stopped.");
            break;

        default:
            this->_statusmessage = "";
            break;
    }

    emit statusMessageChanged();
}
