#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    fileModel = new QFileSystemModel(this);
    audioPlayer = new AudioPlayer(this);

    setupFileSystemModel();
    setupAudioPlayer();
    setupConnections();

    loadDirectory("C:/AudioPlayerQt/AudioPlayerQt/music");


    ui->verticalSlider->setValue(50);  // Громкость
    audioPlayer->setVolume(50);        // громкость в плеере
    ui->horizontalSlider->setValue(0); // Позиция 0%
    ui->pushButton->setText("▶");      // Кнопка воспроизведения

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupFileSystemModel()
{
    //фильтр на аудио
    audioFilters << "*.mp3" << "*.wav" << "*.ogg" << "*.flac"
                 << "*.m4a" << "*.aac" << "*.wma";

    fileModel->setNameFilters(audioFilters);
    fileModel->setNameFilterDisables(false);

    fileModel->setRootPath("");

    ui->listViewDir->setModel(fileModel);
    ui->listViewDir->setViewMode(QListView::ListMode);
    ui->listViewDir->setSelectionMode(QAbstractItemView::SingleSelection);
}

void MainWindow::setupAudioPlayer()
{

    connect(audioPlayer, &AudioPlayer::positionChanged, this, &MainWindow::updatePosition);
    connect(audioPlayer, &AudioPlayer::playbackStateChanged, this, &MainWindow::updatePlaybackState);
    connect(audioPlayer, &AudioPlayer::volumeChanged, this, &MainWindow::updateVolume);
    connect(audioPlayer, &AudioPlayer::errorOccurred,[](const QString &error)
            {
                qDebug() << "Ошибка воспроизведения:" << error;
            });
}

void MainWindow::setupConnections()
{
    //двойной клик
    connect(ui->listViewDir, &QListView::doubleClicked, this, &MainWindow::onListViewDoubleClicked);

    //кнопка
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onPlayPauseClicked);

    //слайдер громкости
    connect(ui->verticalSlider, &QSlider::valueChanged, this, &MainWindow::onVolumeChanged);

    //слайдер позиции воспроизведения
    connect(ui->horizontalSlider, &QSlider::sliderPressed, this, &MainWindow::onPositionSliderPressed);
    connect(ui->horizontalSlider, &QSlider::sliderReleased, this, &MainWindow::onPositionSliderReleased);
    connect(ui->horizontalSlider, &QSlider::sliderMoved, this, &MainWindow::onPositionSliderMoved);
}

void MainWindow::onListViewDoubleClicked(const QModelIndex &index)
{
    QString filePath = fileModel->filePath(index);
    QFileInfo fileInfo(filePath);

    qDebug() << "Выбран файл:" << filePath;

    if (fileInfo.isDir()) {
        // Если это папка - загружаем ее содержимое
        loadDirectory(filePath);
    } else if (fileInfo.isFile()) {
        // Если это файл - воспроизводим его
        playAudioFile(filePath);
    }
}

void MainWindow::loadDirectory(const QString &path)
{
    ui->listViewDir->setRootIndex(fileModel->index(path));
}

void MainWindow::playAudioFile(const QString &filePath)
{
    //источник
    audioPlayer->setSource(QUrl::fromLocalFile(filePath));

    // Обновление информации о треке
    updateMediaInfo();

    // Воспроизведение
    audioPlayer->play();
}

void MainWindow::onPlayPauseClicked()
{
    if (audioPlayer->isPlaying()) {
        audioPlayer->pause();
    }
    audioPlayer->play();
}

void MainWindow::onVolumeChanged(int value)
{
    audioPlayer->setVolume(value);
}

void MainWindow::onPositionSliderPressed()
{
    // При нажатии на слайдер временно отключаем обновление позиции
    disconnect(audioPlayer, &AudioPlayer::positionChanged,
               this, &MainWindow::updatePosition);
}

void MainWindow::onPositionSliderReleased()
{
    // При отпускании слайдера устанавливаем новую позицию
    if (audioPlayer->duration() > 0) {
        qint64 newPosition = static_cast<qint64>(
            ui->horizontalSlider->value() * audioPlayer->duration() / 100.0);
        audioPlayer->setPosition(newPosition);
    }

    // Возвращаем подключение сигнала
    connect(audioPlayer, &AudioPlayer::positionChanged,
            this, &MainWindow::updatePosition);
}

void MainWindow::onPositionSliderMoved(int value)
{
    // Обновляем время при перемещении слайдера
    if (audioPlayer->duration() > 0) {
        qint64 position = static_cast<qint64>(value * audioPlayer->duration() / 100.0);
        ui->trackDuration->setText(formatTime(position));
    }
}

void MainWindow::updatePosition(qint64 position)
{
    if (audioPlayer->duration() > 0) {
        // Обновляем позицию слайдера
        int sliderValue = static_cast<int>(position * 100.0 / audioPlayer->duration());
        ui->horizontalSlider->setValue(sliderValue);

        // Обновляем время в статусбаре
        QString timeStr = formatTime(position) + " / " +
                          formatTime(audioPlayer->duration());
        ui->trackDuration->setText(timeStr);
    }
}


void MainWindow::updatePlaybackState(QMediaPlayer::PlaybackState state)
{
    if (state == QMediaPlayer::PlayingState) {
        ui->pushButton->setText("⏸");
        // Обновляем заголовок окна с названием трека
        QString fileName = getFileNameWithoutExtension(audioPlayer->currentSource().toLocalFile());
        if (!fileName.isEmpty()) {
            setWindowTitle(fileName + " - Аудиоплеер");
        }
    } else {
        ui->pushButton->setText("▶");
        if (state == QMediaPlayer::StoppedState) {
            ui->horizontalSlider->setValue(0);
            ui->trackDuration->clear();
        }
    }
}

void MainWindow::updateVolume(int volume)
{
    // Синхронизация слайдера громкости с состоянием плеера
    if (ui->verticalSlider->value() != volume) {
        ui->verticalSlider->setValue(volume);
    }
}

void MainWindow::updateMediaInfo()
{
    // Обновление информации о текущем треке
    QString filePath = audioPlayer->currentSource().toLocalFile();
    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();


        ui->trackName->setText(fileName);

        qDebug() << "Воспроизводится:" << fileName;
    }
}

QString MainWindow::formatTime(qint64 milliseconds)
{
    qint64 totalSeconds = milliseconds / 1000;
    qint64 minutes = totalSeconds / 60;
    qint64 seconds = totalSeconds % 60;

    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

QString MainWindow::getFileNameWithoutExtension(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    return fileInfo.baseName();
}
