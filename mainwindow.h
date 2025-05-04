#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QGroupBox>
#include <memory>
#include <array>
#include "videoplayer.h"

namespace MOOP {

class VideoPlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlayerWidget(QWidget *parent = nullptr);

    std::shared_ptr<VideoPlayer> getVideoPlayer() const { return m_videoPlayer; }

private slots:
    void onOpenFile();
    void onPlay();
    void onPause();
    void onRestart();
    void updateFrame(const QImage &frame);
    void handleError(const QString &error);

private:
    QLabel *m_videoLabel;
    QLabel *m_nameLabel;
    QPushButton *m_openButton;
    QPushButton *m_playButton;
    QPushButton *m_pauseButton;
    QPushButton *m_restartButton;
    std::shared_ptr<VideoPlayer> m_videoPlayer;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void playAllVideos();
    void pauseAllVideos();
    void restartAllVideos();

private:
    QWidget *m_centralWidget;
    QGridLayout *m_mainLayout;
    std::array<VideoPlayerWidget*, 4> m_videoWidgets;

    QPushButton *m_playAllButton;
    QPushButton *m_pauseAllButton;
    QPushButton *m_restartAllButton;
};

} // namespace MOOP

#endif // MAINWINDOW_H
