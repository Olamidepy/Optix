#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <memory>
#include "UI/Common/Sidebar.hpp"
#include "UI/Views/LoginView.hpp"
#include "UI/Views/OnboardingView.hpp"
#include "UI/Views/DashboardView.hpp"
#include "UI/Views/StudentManagementView.hpp"
#include "UI/Views/FaceRegistrationView.hpp"
#include "UI/Views/AttendanceView.hpp"
#include "UI/Views/ReportsView.hpp"
#include "UI/Views/SettingsView.hpp"

namespace Optix::Application {
class AppContext;
}

namespace Optix::Application {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(std::shared_ptr<AppContext> context, QWidget* parent = nullptr);
    ~MainWindow() override = default;

    // Evaluates state (database content, login session) and forces the appropriate screen
    void evaluateAppFlow();

private slots:
    void handleNavigation(int index);
    void handleLogout();

private:
    void setupUI();
    void connectSignals();

    std::shared_ptr<AppContext> m_context;

    // Layout elements
    UI::Common::Sidebar* m_sidebar{nullptr};
    QStackedWidget* m_stackedWidget{nullptr};

    // Sub-view references inside QStackedWidget
    UI::Views::LoginView* m_loginView{nullptr};
    UI::Views::OnboardingView* m_onboardingView{nullptr};
    UI::Views::DashboardView* m_dashboardView{nullptr};
    UI::Views::StudentManagementView* m_studentView{nullptr};
    UI::Views::FaceRegistrationView* m_faceView{nullptr};
    UI::Views::AttendanceView* m_attendanceView{nullptr};
    UI::Views::ReportsView* m_reportsView{nullptr};
    UI::Views::SettingsView* m_settingsView{nullptr};
};

} // namespace Optix::Application
