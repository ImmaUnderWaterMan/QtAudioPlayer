#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QAudioOutput>
#include <QUrl>
#include <QTime>
#include <QPixmap>

class AudioPlayer : public QObject
{
    Q_OBJECT

public:
    explicit AudioPlayer(QObject *parent = nullptr);
    ~AudioPlayer();


    void play();
    void pause();
    void stop();
    void setVolume(int volume);
    void setPosition(qint64 position);
    void setSource(const QUrl &url);
    void setPlaybackRate(qreal rate);
    void setTrackPic();

    bool isPlaying() const;
    int volume() const;
    qint64 duration() const;
    qint64 position() const;
    QMediaPlayer::PlaybackState playbackState() const;
    QMediaPlayer::MediaStatus mediaStatus() const;
    QUrl currentSource() const;


    static QString formatTime(qint64 milliseconds);

signals:
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void volumeChanged(int volume);
    void errorOccurred(const QString &errorString);
    void sourceChanged(const QUrl &url);
    void trackCoverChanged(const QPixmap &cover);

private slots:
    void handleErrorOccurred();

private:
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
    QUrl m_currentSource;
    QPixmap m_trackCover;
};

#endif // PLAYER_H
