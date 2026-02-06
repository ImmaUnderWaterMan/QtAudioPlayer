#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include "player.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onListViewDoubleClicked(const QModelIndex &index);
    void loadDirectory(const QString &path);

    void onPlayPauseClicked();
    void onVolumeChanged(int value);
    void onPositionSliderPressed();
    void onPositionSliderReleased();
    void onPositionSliderMoved(int value);


    void updatePosition(qint64 position);
    void updatePlaybackState(QMediaPlayer::PlaybackState state);
    void updateVolume(int volume);
    void updateMediaInfo();
private:
    Ui::MainWindow *ui;
    QFileSystemModel *fileModel;
    AudioPlayer *audioPlayer;

    QStringList audioFilters;



    void setupConnections();
    void setupFileSystemModel();
    void setupAudioPlayer();
    void playAudioFile(const QString &filePath);

    QString formatTime(qint64 milliseconds);
    QString getFileNameWithoutExtension(const QString &filePath);
};

#endif // MAINWINDOW_H
