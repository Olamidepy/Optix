#pragma once

#include <QObject>
#include <QImage>
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

    void setCascadePath(const std::string& path);

public slots:
    void startCapturing();
    void stopCapturing();

signals:
    // Emitted every frame (~30 FPS) with the RGB preview image (face rectangles drawn)
    void frameReady(const QImage& frameImage);

    // Emitted whenever a face ROI is detected and cropped (200x200 grayscale QImage)
    void faceDetected(const QImage& faceRoiImage);

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
