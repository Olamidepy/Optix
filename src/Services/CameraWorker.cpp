#include "CameraWorker.hpp"
#include <QThread>
#include <QDebug>
#include <filesystem>

#ifndef OPTIX_MOCK_BACKEND
#include <opencv2/imgproc.hpp>
#endif

namespace fs = std::filesystem;

namespace Optix::Services {

CameraWorker::CameraWorker(int deviceIndex, QObject* parent)
    : QObject(parent), m_deviceIndex(deviceIndex) {
}

CameraWorker::~CameraWorker() {
    stopCapturing();
}

void CameraWorker::setCascadePath(const std::string& path) {
    m_cascadePath = path;
}

void CameraWorker::startCapturing() {
    m_running = true;

#ifdef OPTIX_MOCK_BACKEND
    qDebug() << "CameraWorker: Starting mock camera feed.";
    while (m_running) {
        QThread::msleep(33); // ~30 FPS
    }
    emit cameraStopped();
#else
    qDebug() << "CameraWorker: Opening webcam device index" << m_deviceIndex;
    
    // Try DirectShow first (fastest on Windows MinGW), then MSMF, then ANY
    bool opened = m_capture.open(m_deviceIndex, cv::CAP_DSHOW);
    if (!opened) {
        opened = m_capture.open(m_deviceIndex, cv::CAP_MSMF);
    }
    if (!opened) {
        opened = m_capture.open(m_deviceIndex, cv::CAP_ANY);
    }

    if (!opened) {
        qWarning() << "CameraWorker: Could not open camera device" << m_deviceIndex;
        emit cameraError("Failed to open camera device index " + QString::number(m_deviceIndex));
        m_running = false;
        emit cameraStopped();
        return;
    }

    // Set resolution to standard 640x480 for fast capture
    m_capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    m_capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    // Attempt to load cascade classifier
    std::string cascadeToUse = m_cascadePath;
    if (!fs::exists(cascadeToUse)) {
        std::vector<std::string> searchPaths = {
            "models/haarcascade_frontalface_default.xml",
            "C:/Users/ACER/Downloads/OpenCV_MingW-main/OpenCV_MingW-main/OpenCV4.11_mingw13.1.0/etc/haarcascades/haarcascade_frontalface_default.xml"
        };
        for (const auto& p : searchPaths) {
            if (fs::exists(p)) {
                cascadeToUse = p;
                break;
            }
        }
    }

    if (!cascadeToUse.empty() && fs::exists(cascadeToUse)) {
        m_faceCascade.load(cascadeToUse);
        qDebug() << "CameraWorker: Loaded Haar Cascade from" << QString::fromStdString(cascadeToUse);
    } else {
        qWarning() << "CameraWorker: Haar Cascade classifier file not found. Face detection will be bypassed.";
    }

    cv::Mat frame;
    cv::Mat grayFrame;

    while (m_running) {
        if (!m_capture.read(frame) || frame.empty()) {
            QThread::msleep(10);
            continue;
        }

        // Convert frame to grayscale for detection
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(grayFrame, grayFrame);

        // Detect faces
        std::vector<cv::Rect> faces;
        if (!m_faceCascade.empty()) {
            m_faceCascade.detectMultiScale(
                grayFrame, faces, 1.1, 4, 
                0 | cv::CASCADE_SCALE_IMAGE, 
                cv::Size(60, 60)
            );
        }

        // Create display image with orange bounding boxes
        cv::Mat displayFrame = frame.clone();
        for (const auto& rect : faces) {
            // Draw sleek orange box around face (BGR: #F97316 -> 22, 115, 249)
            cv::rectangle(displayFrame, rect, cv::Scalar(22, 115, 249), 2, cv::LINE_AA);
        }

        // Convert OpenCV BGR to Qt RGB QImage
        cv::Mat rgbFrame;
        cv::cvtColor(displayFrame, rgbFrame, cv::COLOR_BGR2RGB);
        QImage qimg(
            rgbFrame.data, 
            rgbFrame.cols, 
            rgbFrame.rows, 
            static_cast<int>(rgbFrame.step), 
            QImage::Format_RGB888
        );

        emit frameCaptured(qimg.copy(), frame, faces);

        QThread::msleep(33); // ~30 FPS
    }

    m_capture.release();
    qDebug() << "CameraWorker: Camera capture thread stopped.";
    emit cameraStopped();
#endif
}

void CameraWorker::stopCapturing() {
    m_running = false;
}

} // namespace Optix::Services
