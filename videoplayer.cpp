#include "videoplayer.h"
#include <QFileInfo>
#include <QDebug>

namespace MOOP {

VideoPlayer::VideoPlayer(QObject *parent)
    : QObject(parent)
{
}

VideoPlayer::~VideoPlayer()
{
    stopThread();
    if (m_videoCapture.isOpened()) {
        m_videoCapture.release();
    }
}

bool VideoPlayer::openFile(const QString &fileName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_videoCapture.open(fileName.toStdString());
    if (!m_videoCapture.isOpened()) {
        emit errorOccurred("Failed to open video file.");
        return false;
    }

    m_videoName = QFileInfo(fileName).fileName();
    m_isOpened = true;
    return true;
}

void VideoPlayer::play()
{
    if (m_isOpened) {
        m_isPlaying = true;
        if (!m_thread) {
            m_thread = std::make_unique<std::thread>(&VideoPlayer::threadFunction, this);
        }
    }
}

void VideoPlayer::pause()
{
    m_isPlaying = false;
}

void VideoPlayer::restart()
{
    if (m_isOpened) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_videoCapture.set(cv::CAP_PROP_POS_FRAMES, 0);
        m_isPlaying = true;
    }
}

void VideoPlayer::stopThread()
{
    m_stopThread = true;
    m_isPlaying = false;

    if (m_thread && m_thread->joinable()) {
        m_thread->join();
        m_thread.reset();
    }
}

void VideoPlayer::threadFunction()
{
    cv::Mat frame;
    const int frameDelay = static_cast<int>(1000 / m_videoCapture.get(cv::CAP_PROP_FPS));

    while (!m_stopThread) {
        if (m_isPlaying) {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (!m_videoCapture.read(frame)) {
                    m_isPlaying = false;
                    emit playbackFinished();
                    continue;
                }
            }

            // Convert frame to QImage
            cv::Mat rgbFrame;
            cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);

            QImage image(
                rgbFrame.data,
                rgbFrame.cols,
                rgbFrame.rows,
                rgbFrame.step,
                QImage::Format_RGB888
            );

            // Emit the frame to the GUI
            emit frameReady(image.copy());

            // Sleep to maintain frame rate
            std::this_thread::sleep_for(std::chrono::milliseconds(frameDelay));
        } else {
            // Reduce CPU usage when paused
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

} // namespace MOOP
