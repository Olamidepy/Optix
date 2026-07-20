#pragma once

#include <QObject>
#include <QImage>
#include <QMutex>
#include <memory>
#include <vector>
#include <string>

#ifndef OPTIX_MOCK_BACKEND
#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/videoio.hpp>
#endif

namespace Optix::Services {

class CameraWorker : public QObject {
    Q_OBJECT

public:
    explicit CameraWorker(int deviceIndex = 0, QObject* parent = nullptr);
    ~CameraWorker() override;

    // Set cascade path for face detection
    void setCascadePath(const std::string& path);

public slots:
    void startCapturing();
    void stopCapturing();

signals:
    // Emitted every time a new video frame is processed
    // frameImage: RGB image ready for Qt QLabel preview (with bounding boxes drawn)
    // rawGrayFrame: Processed grayscale matrix for face recognition/saving
    // faceRois: Cropped face bounding box matrices
    void frameReady(const QImage& frameImage, const std::vector<unsigned char>& rawJpegBuffer);
    
#ifndef OPTIX_MOCK_BACKEND
    void frameCaptured(const QImage& displayImage, const cv::Mat& rawFrame, const std::vector<cv::Rect>& faceRects);
#endif

    void cameraError(const QString& errorMessage);
    void cameraStopped();

private:
    int m_deviceIndex{0};
    bool m_running{false};
    std::string m_cascadePath{"models/haarcascade_frontalface_default.xml"};

#ifndef OPTIX_MOCK_BACKEND
    cv::VideoCapture m_capture;
    cv::CascadeClassifier m_faceCascade;
#endif
};

} // namespace Optix::Services
