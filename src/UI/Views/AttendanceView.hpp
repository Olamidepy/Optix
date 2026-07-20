#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QThread>
#include <QDateTime>
#include <memory>
#include <map>
#include <string>

namespace Optix::Application {
class AppContext;
}

namespace Optix::Services {
class CameraWorker;
}

namespace Optix::UI::Views {

class AttendanceView : public QWidget {
    Q_OBJECT

public:
    explicit AttendanceView(std::shared_ptr<Application::AppContext> context, QWidget* parent = nullptr);
    ~AttendanceView() override;

private slots:
    void toggleSession();
    void simulateVerification(); // Mock fallback verification trigger

private:
    void setupUI();
    void refreshSessionLogs();
    void stopCameraThread();

    std::shared_ptr<Application::AppContext> m_context;

    QLabel* m_cameraPreview{nullptr};
    QPushButton* m_sessionToggleBtn{nullptr};
    QPushButton* m_simulateVerifyBtn{nullptr};
    QLabel* m_statusLbl{nullptr};
    QTableWidget* m_todayLogsTable{nullptr};

    bool m_isSessionRunning{false};

    QThread* m_cameraThread{nullptr};
    Services::CameraWorker* m_cameraWorker{nullptr};

    // Cooldown map to avoid rapid duplicate recognition events in the same session
    std::map<std::string, QDateTime> m_lastRecognitionTime;
};

} // namespace Optix::UI::Views
