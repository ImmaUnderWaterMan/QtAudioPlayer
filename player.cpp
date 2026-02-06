#include "player.h"
#include <QDebug>

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
    , m_player(new QMediaPlayer(this))
    , m_audioOutput(new QAudioOutput(this))
{
    // Настройка аудиовыхода
    m_player->setAudioOutput(m_audioOutput);

    connect(m_player, &QMediaPlayer::positionChanged, this, &AudioPlayer::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &AudioPlayer::durationChanged);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &AudioPlayer::playbackStateChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &AudioPlayer::mediaStatusChanged);

    // Подключение для обработки ошибок
    connect(m_player, &QMediaPlayer::errorOccurred, this, &AudioPlayer::handleErrorOccurred);
}

AudioPlayer::~AudioPlayer()
{
    stop();
}

void AudioPlayer::play()
{
    if (m_player->playbackState() == QMediaPlayer::PausedState)
    {
        m_player->play();
    }
    else if (m_player->mediaStatus() == QMediaPlayer::NoMedia)
    {
        if (!m_currentSource.isEmpty())
        {
            setSource(m_currentSource);
            m_player->play();
        }
    }
    else
    {
        m_player->play();
    }
}

void AudioPlayer::pause()
{
    m_player->pause();
}

void AudioPlayer::stop()
{
    m_player->stop();
}

void AudioPlayer::setVolume(int volume)
{
    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;

    m_audioOutput->setVolume(volume / 100.0);
    emit volumeChanged(volume);
}

void AudioPlayer::setPosition(qint64 position)
{
    m_player->setPosition(position);
}

void AudioPlayer::setSource(const QUrl &url)
{
    if (url != m_currentSource) {
        m_currentSource = url;
        m_player->setSource(url);
        emit sourceChanged(url);
    }
}

void AudioPlayer::setPlaybackRate(qreal rate)
{
    m_player->setPlaybackRate(rate);
}

bool AudioPlayer::isPlaying() const
{
    return m_player->playbackState() == QMediaPlayer::PlayingState;
}

int AudioPlayer::volume() const
{
    return static_cast<int>(m_audioOutput->volume() * 100);
}

qint64 AudioPlayer::duration() const
{
    return m_player->duration();
}

qint64 AudioPlayer::position() const
{
    return m_player->position();
}

QMediaPlayer::PlaybackState AudioPlayer::playbackState() const
{
    return m_player->playbackState();
}

QMediaPlayer::MediaStatus AudioPlayer::mediaStatus() const
{
    return m_player->mediaStatus();
}

QUrl AudioPlayer::currentSource() const
{
    return m_currentSource;
}

QString AudioPlayer::formatTime(qint64 milliseconds)
{
    QTime time(0, 0, 0);
    time = time.addMSecs(milliseconds);

    if (time.hour() > 0) {
        return time.toString("hh:mm:ss");
    } else {
        return time.toString("mm:ss");
    }
}

void AudioPlayer::handleErrorOccurred()
{
    QString errorString = m_player->errorString();
    emit errorOccurred(errorString);
    qDebug() << "AudioPlayer error:" << errorString;
}
