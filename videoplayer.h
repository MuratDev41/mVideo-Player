#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <QImage>
#include <QString>
#include <opencv2/opencv.hpp>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

namespace MOOP {

class VideoPlayer : public QObject
{
    Q_OBJECT

public:
    explicit VideoPlayer(QObject *parent = nullptr);
    ~VideoPlayer();

    bool openFile(const QString &fileName);
    void play();
    void pause();
    void restart();

    QString getVideoName() const { return m_videoName; }
    bool isPlaying() const { return m_isPlaying; }
    bool isOpened() const { return m_isOpened; }

signals:
    void frameReady(const QImage &frame);
    void playbackFinished();
    void errorOccurred(const QString &error);

public slots:
    void stopThread();

private:
    void threadFunction();

    QString m_videoName;
    QString m_filePath;
    cv::VideoCapture m_videoCapture;

    std::atomic<bool> m_isPlaying{false};
    std::atomic<bool> m_isOpened{false};
    std::atomic<bool> m_stopThread{false};

    std::unique_ptr<std::thread> m_thread;
    std::mutex m_mutex;
};

} // namespace MOOP

#endif // VIDEOPLAYER_H
