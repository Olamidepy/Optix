#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <memory>

namespace Optix::Application {
class AppContext;
}

namespace Optix::UI::Views {

class AttendanceView : public QWidget {
    Q_OBJECT

public:
    explicit AttendanceView(std::shared_ptr<Application::AppContext> context, QWidget* parent = nullptr);
    ~AttendanceView() override = default;

private slots:
    void toggleSession();
    void simulateVerification(); // Mock verification stub

private:
    void setupUI();
    void refreshSessionLogs();

    std::shared_ptr<Application::AppContext> m_context;

    QLabel* m_cameraPreview{nullptr};
    QPushButton* m_sessionToggleBtn{nullptr};
    QPushButton* m_simulateVerifyBtn{nullptr};
    QLabel* m_statusLbl{nullptr};
    QTableWidget* m_todayLogsTable{nullptr};

    bool m_isSessionRunning{false};
};

} // namespace Optix::UI::Views
