#include "MainWindow.hpp"
#include "AppContext.hpp"
#include <QHBoxLayout>
#include <QFrame>
#include <QMessageBox>

namespace Optix::Application {

MainWindow::MainWindow(std::shared_ptr<AppContext> context, QWidget* parent)
    : QMainWindow(parent), m_context(std::move(context)) {
    setupUI();
    connectSignals();
    evaluateAppFlow();
    
    // Apply initial theme stylesheet
    setStyleSheet(m_context->themeManager->getStyleSheet());
}

void MainWindow::setupUI() {
    setWindowTitle("Optix - Face Recognition Attendance Management");
    resize(1200, 800);
    setMinimumSize(960, 640);

    // Global layout container widget
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setObjectName("CentralWidget");
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. Sidebar Left Panel
    m_sidebar = new UI::Common::Sidebar(m_context, centralWidget);
    mainLayout->addWidget(m_sidebar);

    // 2. Stacked Content Right Panel
    m_stackedWidget = new QStackedWidget(centralWidget);
    m_stackedWidget->setObjectName("MainStack");
    mainLayout->addWidget(m_stackedWidget, 1);

    // Instantiate View screens
    m_dashboardView = new UI::Views::DashboardView(m_context, m_stackedWidget);
    m_studentView = new UI::Views::StudentManagementView(m_context, m_stackedWidget);
    m_faceView = new UI::Views::FaceRegistrationView(m_context, m_stackedWidget);
    m_attendanceView = new UI::Views::AttendanceView(m_context, m_stackedWidget);
    m_reportsView = new UI::Views::ReportsView(m_context, m_stackedWidget);
    m_settingsView = new UI::Views::SettingsView(m_context, m_stackedWidget);
    m_onboardingView = new UI::Views::OnboardingView(m_context, m_stackedWidget);
    m_loginView = new UI::Views::LoginView(m_context, m_stackedWidget);

    // Add views to stacked container matching index
    m_stackedWidget->insertWidget(0, m_dashboardView);
    m_stackedWidget->insertWidget(1, m_studentView);
    m_stackedWidget->insertWidget(2, m_faceView);
    m_stackedWidget->insertWidget(3, m_attendanceView);
    m_stackedWidget->insertWidget(4, m_reportsView);
    m_stackedWidget->insertWidget(5, m_settingsView);
    m_stackedWidget->insertWidget(6, m_onboardingView);
    m_stackedWidget->insertWidget(7, m_loginView);
}

void MainWindow::connectSignals() {
    // Left Sidebar navigation
    connect(m_sidebar, &UI::Common::Sidebar::navigationRequested, this, &MainWindow::handleNavigation);
    connect(m_sidebar, &UI::Common::Sidebar::logoutRequested, this, &MainWindow::handleLogout);

    // Login screen transitions
    connect(m_loginView, &UI::Views::LoginView::loginSuccess, this, &MainWindow::evaluateAppFlow);

    // Onboarding screen transitions
    connect(m_onboardingView, &UI::Views::OnboardingView::registerStudentsClicked, this, [this]() {
        m_sidebar->show();
        handleNavigation(1); // Switch to Student Directory Management tab
    });

    // Student directory → Face registration linkage
    connect(m_studentView, &UI::Views::StudentManagementView::registerFaceRequested, this, [this](const QString& studentId) {
        m_faceView->setTargetStudent(studentId);
        m_sidebar->setSelectedPage(2);
        handleNavigation(2); // Switch to Face Registration tab
    });
}

void MainWindow::evaluateAppFlow() {
    if (!m_context->authService->isLoggedIn()) {
        m_sidebar->hide();
        m_stackedWidget->setCurrentIndex(7); // Login screen
        return;
    }

    int studentCount = m_context->studentService->getStudentCount();
    if (studentCount == 0) {
        m_sidebar->hide();
        m_stackedWidget->setCurrentIndex(6); // Onboarding screen
    } else {
        m_sidebar->show();
        m_sidebar->setSelectedPage(0);
        handleNavigation(0); // Show Dashboard panel
    }
}

void MainWindow::handleNavigation(int index) {
    if (!m_context->authService->isLoggedIn()) return;

    m_stackedWidget->setCurrentIndex(index);
    m_sidebar->setSelectedPage(index);

    // Refresh dynamic content on tab show
    switch (index) {
        case 0:
            m_dashboardView->refreshStats();
            break;
        case 1:
            m_studentView->refreshStudentTable();
            break;
        case 4:
            m_reportsView->refreshReport();
            break;
        default:
            break;
    }
}

void MainWindow::handleLogout() {
    auto reply = QMessageBox::question(
        this, "Sign Out", "Are you sure you want to log out of the dashboard?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        m_context->authService->logout();
        evaluateAppFlow();
    }
}

} // namespace Optix::Application
