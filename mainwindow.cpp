#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace MOOP {

// VideoPlayerWidget Implementation
VideoPlayerWidget::VideoPlayerWidget(QWidget *parent)
    : QWidget(parent)
{
    // Video oynatıcı nesnesini oluştur
    m_videoPlayer = std::make_shared<VideoPlayer>();

    // Video ekranı için label
    m_videoLabel = new QLabel(this);
    m_videoLabel->setMinimumSize(320, 240);
    m_videoLabel->setAlignment(Qt::AlignCenter);
    m_videoLabel->setStyleSheet("background-color: black;");

    // Video ismi için label
    m_nameLabel = new QLabel("Video Seçilmedi", this);
    m_nameLabel->setAlignment(Qt::AlignCenter);

    // Kontrol butonları
    m_openButton = new QPushButton("Dosya Aç", this);
    m_playButton = new QPushButton("Başlat", this);
    m_pauseButton = new QPushButton("Duraklat", this);
    m_restartButton = new QPushButton("Yeniden Başlat", this);

    // Butonları devre dışı bırak (başlangıçta dosya seçilmediği için)
    m_playButton->setEnabled(false);
    m_pauseButton->setEnabled(false);
    m_restartButton->setEnabled(false);

    // Buton düzeni
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_openButton);
    buttonLayout->addWidget(m_playButton);
    buttonLayout->addWidget(m_pauseButton);
    buttonLayout->addWidget(m_restartButton);

    // Ana düzen
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_videoLabel, 1);
    layout->addWidget(m_nameLabel);
    layout->addLayout(buttonLayout);

    // Sinyal-slot bağlantıları
    connect(m_openButton, &QPushButton::clicked, this, &VideoPlayerWidget::onOpenFile);
    connect(m_playButton, &QPushButton::clicked, this, &VideoPlayerWidget::onPlay);
    connect(m_pauseButton, &QPushButton::clicked, this, &VideoPlayerWidget::onPause);
    connect(m_restartButton, &QPushButton::clicked, this, &VideoPlayerWidget::onRestart);

    connect(m_videoPlayer.get(), &VideoPlayer::frameReady, this, &VideoPlayerWidget::updateFrame);
    connect(m_videoPlayer.get(), &VideoPlayer::errorOccurred, this, &VideoPlayerWidget::handleError);
}

void VideoPlayerWidget::onOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Video Dosyası Aç",
                                                    QString(),
                                                    "Video Dosyaları (*.mp4 *.avi *.mkv *.mov);;Tüm Dosyalar (*)");

    if (fileName.isEmpty()) {
        return;
    }

    if (m_videoPlayer->openFile(fileName)) {
        m_nameLabel->setText(m_videoPlayer->getVideoName());
        m_playButton->setEnabled(true);
        m_pauseButton->setEnabled(true);
        m_restartButton->setEnabled(true);
    }
}

void VideoPlayerWidget::onPlay()
{
    m_videoPlayer->play();
}

void VideoPlayerWidget::onPause()
{
    m_videoPlayer->pause();
}

void VideoPlayerWidget::onRestart()
{
    m_videoPlayer->restart();
}

void VideoPlayerWidget::updateFrame(const QImage &frame)
{
    QPixmap pixmap = QPixmap::fromImage(frame);
    m_videoLabel->setPixmap(pixmap.scaled(m_videoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void VideoPlayerWidget::handleError(const QString &error)
{
    QMessageBox::warning(this, "Hata", error);
}

// MainWindow Implementation
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Çoklu Video Oynatıcı");
    resize(1024, 768);

    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QGridLayout(m_centralWidget);

    // 2x2 düzeninde 4 video oynatıcı oluştur
    for (int i = 0; i < 4; ++i) {
        m_videoWidgets[i] = new VideoPlayerWidget(this);
        m_mainLayout->addWidget(m_videoWidgets[i], i / 2, i % 2);
    }

    // Global kontrol butonları
    m_playAllButton = new QPushButton("Tümünü Başlat", this);
    m_pauseAllButton = new QPushButton("Tümünü Duraklat", this);
    m_restartAllButton = new QPushButton("Tümünü Yeniden Başlat", this);

    QHBoxLayout *globalButtonLayout = new QHBoxLayout();
    globalButtonLayout->addWidget(m_playAllButton);
    globalButtonLayout->addWidget(m_pauseAllButton);
    globalButtonLayout->addWidget(m_restartAllButton);

    m_mainLayout->addLayout(globalButtonLayout, 2, 0, 1, 2);

    // Sinyal-slot bağlantıları
    connect(m_playAllButton, &QPushButton::clicked, this, &MainWindow::playAllVideos);
    connect(m_pauseAllButton, &QPushButton::clicked, this, &MainWindow::pauseAllVideos);
    connect(m_restartAllButton, &QPushButton::clicked, this, &MainWindow::restartAllVideos);
}

MainWindow::~MainWindow()
{
}

void MainWindow::playAllVideos()
{
    for (auto widget : m_videoWidgets) {
        if (widget->getVideoPlayer()->isOpened()) {
            widget->getVideoPlayer()->play();
        }
    }
}

void MainWindow::pauseAllVideos()
{
    for (auto widget : m_videoWidgets) {
        if (widget->getVideoPlayer()->isPlaying()) {
            widget->getVideoPlayer()->pause();
        }
    }
}

void MainWindow::restartAllVideos()
{
    for (auto widget : m_videoWidgets) {
        if (widget->getVideoPlayer()->isOpened()) {
            widget->getVideoPlayer()->restart();
        }
    }
}

} // namespace MOOP
