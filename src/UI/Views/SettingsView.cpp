#include "SettingsView.hpp"
#include "Application/AppContext.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QSlider>
#include <QFormLayout>
#include <QPushButton>

namespace Optix::UI::Views {

SettingsView::SettingsView(std::shared_ptr<Application::AppContext> context, QWidget* parent)
    : QWidget(parent), m_context(std::move(context)) {
    setupUI();
    updateThemeBtnText();
}

void SettingsView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(24);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Title Section
    QVBoxLayout* headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(4);
    QLabel* titleLbl = new QLabel("Settings", this);
    titleLbl->setObjectName("HeaderTitle");
    QLabel* subtitleLbl = new QLabel("Configure system parameters, themes, and camera devices", this);
    subtitleLbl->setObjectName("HeaderSubtitle");
    headerLayout->addWidget(titleLbl);
    headerLayout->addWidget(subtitleLbl);
    mainLayout->addLayout(headerLayout);

    // Settings Panel Card
    QFrame* settingsCard = new QFrame(this);
    settingsCard->setObjectName("PanelCard");
    
    QFormLayout* formLayout = new QFormLayout(settingsCard);
    formLayout->setSpacing(20);
    formLayout->setContentsMargins(24, 24, 24, 24);

    // 1. Theme Configuration Row
    QHBoxLayout* themeRow = new QHBoxLayout();
    m_themeToggleBtn = new QPushButton(settingsCard);
    m_themeToggleBtn->setObjectName("SecondaryBtn");
    m_themeToggleBtn->setCursor(Qt::PointingHandCursor);
    m_themeToggleBtn->setFixedWidth(160);
    
    QLabel* themeDesc = new QLabel("Toggle administrative dashboard design view mode", settingsCard);
    themeDesc->setStyleSheet("color: #6B7280; font-size: 12px;");

    themeRow->addWidget(m_themeToggleBtn);
    themeRow->addWidget(themeDesc);
    formLayout->addRow("Application Theme", themeRow);

    // 2. Camera Device Row
    m_cameraSelectCombo = new QComboBox(settingsCard);
    m_cameraSelectCombo->addItems({
        "Device 0: Integrated Webcam",
        "Device 1: External USB Camera",
        "Device 2: Virtual Camera Link"
    });
    m_cameraSelectCombo->setFixedWidth(280);
    
    QLabel* camDesc = new QLabel("Select active source for attendance verification and face dataset capture", settingsCard);
    camDesc->setStyleSheet("color: #6B7280; font-size: 11px;");
    
    QVBoxLayout* camLayout = new QVBoxLayout();
    camLayout->setSpacing(4);
    camLayout->addWidget(m_cameraSelectCombo);
    camLayout->addWidget(camDesc);
    formLayout->addRow("Video Capture Source", camLayout);

    // 3. Threshold Configuration Row
    QHBoxLayout* threshRow = new QHBoxLayout();
    m_thresholdSlider = new QSlider(Qt::Horizontal, settingsCard);
    m_thresholdSlider->setRange(20, 150);
    m_thresholdSlider->setValue(80); // Default LBPH confidence limit
    m_thresholdSlider->setFixedWidth(200);

    m_thresholdValLbl = new QLabel("80.0", settingsCard);
    m_thresholdValLbl->setStyleSheet("font-weight: bold; color: #F97316; font-size: 14px;");

    QLabel* threshDesc = new QLabel("Adjust cutoff. Lower value is stricter (closer match required). Recommended: 75 - 90", settingsCard);
    threshDesc->setStyleSheet("color: #6B7280; font-size: 11px;");

    threshRow->addWidget(m_thresholdSlider);
    threshRow->addWidget(m_thresholdValLbl);
    threshRow->addWidget(threshDesc);
    formLayout->addRow("Confidence Cutoff", threshRow);

    mainLayout->addWidget(settingsCard);

    // 4. System Information Card
    QFrame* infoCard = new QFrame(this);
    infoCard->setObjectName("PanelCard");
    infoCard->setStyleSheet("background-color: #FAFAFA; border: 1px solid #ECECEC;");
    
    QVBoxLayout* infoLayout = new QVBoxLayout(infoCard);
    infoLayout->setSpacing(8);
    infoLayout->setContentsMargins(20, 20, 20, 20);

    QLabel* appInfoTitle = new QLabel("System Information", infoCard);
    appInfoTitle->setStyleSheet("font-family: 'Space Grotesk'; font-size: 16px; font-weight: bold; color: #1F2937;");
    
    QLabel* detailsLbl = new QLabel(
        "Optix Face Recognition Attendance System v1.0.0\n"
        "Engine: C++20 | Qt 6 Widgets | OpenCV 4.10.0 Contrib\n"
        "Storage: SQLite3 Local Database File\n"
        "Design: Space Grotesk / Inter Typography & Responsive CSS Layouts\n"
        "Developed under clean architecture pattern.",
        infoCard
    );
    detailsLbl->setStyleSheet("font-size: 12px; color: #6B7280; line-height: 18px;");

    infoLayout->addWidget(appInfoTitle);
    infoLayout->addWidget(detailsLbl);
    mainLayout->addWidget(infoCard);

    mainLayout->addStretch();

    // Setup Connections
    connect(m_themeToggleBtn, &QPushButton::clicked, this, &SettingsView::handleThemeToggle);
    connect(m_thresholdSlider, &QSlider::valueChanged, this, &SettingsView::handleThresholdChanged);
    
    // Connect context theme changes to update button text automatically
    connect(m_context->themeManager.get(), &Core::ThemeManager::themeChanged, this, &SettingsView::updateThemeBtnText);
}

void SettingsView::handleThemeToggle() {
    m_context->themeManager->toggleTheme();
}

void SettingsView::handleThresholdChanged(int val) {
    m_thresholdValLbl->setText(QString::number(val) + ".0");
}

void SettingsView::updateThemeBtnText() {
    if (m_context->themeManager->isDarkMode()) {
        m_themeToggleBtn->setText("☀️ Switch to Light");
    } else {
        m_themeToggleBtn->setText("🌙 Switch to Dark");
    }
}

} // namespace Optix::UI::Views
