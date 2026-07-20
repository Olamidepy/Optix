#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QThread>
#include <memory>
#include <vector>

namespace Optix::Application {
class AppContext;
}

namespace Optix::Services {
class CameraWorker;
}

namespace Optix::UI::Views {

class FaceRegistrationView : public QWidget {
    Q_OBJECT

public:
    explicit FaceRegistrationView(std::shared_ptr<Application::AppContext> context, QWidget* parent = nullptr);
    ~FaceRegistrationView() override;

    // Direct registration to a specific student
    void setTargetStudent(const QString& studentId);

private slots:
    void toggleCamera();
    void startCapture();
    void triggerModelTraining();

private:
    void setupUI();
    void updateButtons();
    void stopCameraThread();

    std::shared_ptr<Application::AppContext> m_context;
    QString m_targetStudentId;

    QLabel* m_cameraPreview{nullptr};
    QLabel* m_studentInfoLbl{nullptr};
    
    QPushButton* m_cameraToggleBtn{nullptr};
    QPushButton* m_captureBtn{nullptr};
    QPushButton* m_trainBtn{nullptr};

    QProgressBar* m_captureProgressBar{nullptr};
    QLabel* m_statusLbl{nullptr};

    bool m_isCameraRunning{false};
    bool m_isCapturing{false};
    int m_capturedCount{0};

    QThread* m_cameraThread{nullptr};
    Services::CameraWorker* m_cameraWorker{nullptr};
};

} // namespace Optix::UI::Views
