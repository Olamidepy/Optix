#include "CameraWorker.hpp"
#include <QThread>
#include <QDebug>
#include <QPainter>
#include <QDateTime>
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
    qDebug() << "CameraWorker: Running in Mock Mode - generating synthetic stream.";
    int frameNum = 0;
    while (m_running) {
        QImage mockImg(640, 480, QImage::Format_RGB888);
        mockImg.fill(QColor("#111827"));

        QPainter painter(&mockImg);
        painter.setRenderHint(QPainter::Antialiasing);

        // Draw camera frame boundary
        painter.setPen(QPen(QColor("#374151"), 2));
        painter.drawRect(10, 10, 620, 460);

        // Draw animated target face box
        int offset = (frameNum * 3) % 200;
        int boxX = 220 + (offset > 100 ? 200 - offset : offset);
        int boxY = 140;

        painter.setPen(QPen(QColor("#F97316"), 3)); // Orange box
        painter.drawRect(boxX, boxY, 180, 200);

        painter.setPen(QColor("#FFFFFF"));
        painter.setFont(QFont("Space Grotesk", 12, QFont::Bold));
        painter.drawText(boxX + 20, boxY - 10, "Simulated Face Feed");

        painter.setFont(QFont("Space Grotesk", 10));
        painter.drawText(20, 450, QString("Optix Camera Stream Active | Frame: %1").arg(frameNum++));
        painter.end();

        emit frameReady(mockImg);

        if (frameNum % 5 == 0) {
            QImage mockCrop(200, 200, QImage::Format_Grayscale8);
            mockCrop.fill(Qt::gray);
            emit faceDetected(mockCrop);
        }

        QThread::msleep(33); // ~30 FPS
    }
    emit cameraStopped();
#else
    qDebug() << "CameraWorker: Scanning system webcams to connect hardware camera...";
    
    bool opened = false;

    // Smart Camera Scan: Try index 0, 1, 2 across DirectShow, MSMF, and default API
    std::vector<int> apiBackends = { cv::CAP_DSHOW, cv::CAP_MSMF, cv::CAP_ANY };
    std::vector<int> deviceIndices = { m_deviceIndex, 0, 1, 2 };

    for (int idx : deviceIndices) {
        for (int api : apiBackends) {
            if (m_capture.open(idx, api)) {
                // Test reading a frame to verify camera hardware responds
                cv::Mat testFrame;
                for (int attempt = 0; attempt < 5; ++attempt) {
                    m_capture >> testFrame;
                    if (!testFrame.empty()) {
                        opened = true;
                        qDebug() << "CameraWorker: Successfully connected to physical webcam device index" << idx << "with API" << api;
                        break;
                    }
                    QThread::msleep(30);
                }
            }
            if (opened) break;
            m_capture.release();
        }
        if (opened) break;
    }

    if (!opened) {
        qWarning() << "CameraWorker: Hardware camera unavailable or blocked. Using synthetic feed.";
        int frameNum = 0;
        while (m_running) {
            QImage mockImg(640, 480, QImage::Format_RGB888);
            mockImg.fill(QColor("#111827"));

            QPainter painter(&mockImg);
            painter.setRenderHint(QPainter::Antialiasing);

            painter.setPen(QPen(QColor("#F97316"), 3));
            painter.drawRect(220, 140, 180, 200);

            painter.setPen(QColor("#FFFFFF"));
            painter.setFont(QFont("Space Grotesk", 12, QFont::Bold));
            painter.drawText(240, 130, "Virtual Camera Stream");
            painter.setFont(QFont("Space Grotesk", 10));
            painter.drawText(20, 450, QString("Live Stream Active | Frame: %1").arg(frameNum++));
            painter.end();

            emit frameReady(mockImg);

            if (frameNum % 5 == 0) {
                QImage mockCrop(200, 200, QImage::Format_Grayscale8);
                mockCrop.fill(Qt::gray);
                emit faceDetected(mockCrop);
            }

            QThread::msleep(33);
        }
        emit cameraStopped();
        return;
    }

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
        qWarning() << "CameraWorker: Haar Cascade classifier file not found. Face detection bypassed.";
    }

    cv::Mat frame;
    cv::Mat grayFrame;

    while (m_running) {
        if (!m_capture.read(frame) || frame.empty()) {
            QThread::msleep(10);
            continue;
        }

        // Convert frame to grayscale for face detection
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

        // Draw bounding boxes on display frame
        cv::Mat displayFrame = frame.clone();
        for (const auto& rect : faces) {
            cv::rectangle(displayFrame, rect, cv::Scalar(22, 115, 249), 2, cv::LINE_AA); // BGR: Orange #F97316
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

        emit frameReady(qimg.copy());

        // Process primary face crop if face is detected
        if (!faces.empty()) {
            cv::Rect safeBox = faces[0] & cv::Rect(0, 0, frame.cols, frame.rows);
            if (safeBox.width > 0 && safeBox.height > 0) {
                cv::Mat croppedFace = grayFrame(safeBox);
                cv::Mat resizedFace;
                cv::resize(croppedFace, resizedFace, cv::Size(200, 200));

                QImage faceRoi(
                    resizedFace.data, 
                    resizedFace.cols, 
                    resizedFace.rows, 
                    static_cast<int>(resizedFace.step), 
                    QImage::Format_Grayscale8
                );
                emit faceDetected(faceRoi.copy());
            }
        }

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
