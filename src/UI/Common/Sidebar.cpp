#include "Sidebar.hpp"
#include "Application/AppContext.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>

namespace Optix::UI::Common {

Sidebar::Sidebar(std::shared_ptr<Application::AppContext> context, QWidget* parent)
    : QFrame(parent), m_context(std::move(context)) {
    setupUI();
}

void Sidebar::setupUI() {
    setObjectName("SidebarFrame");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 16);
    layout->setSpacing(0);

    // App Branding Section
    QLabel* titleLbl = new QLabel("Optix", this);
    titleLbl->setObjectName("SidebarTitle");

    QLabel* subtitleLbl = new QLabel("Face Attendance System", this);
    subtitleLbl->setObjectName("SidebarSubtitle");

    layout->addWidget(titleLbl);
    layout->addWidget(subtitleLbl);

    m_btnGroup = new QButtonGroup(this);
    m_btnGroup->setExclusive(true);

    // Navigation Menu Buttons
    QPushButton* dashBtn = createNavButton("Dashboard", "🏠", 0);
    QPushButton* studBtn = createNavButton("Students", "👥", 1);
    QPushButton* faceBtn = createNavButton("Face Registration", "📸", 2);
    QPushButton* attendBtn = createNavButton("Take Attendance", "✅", 3);
    QPushButton* reportBtn = createNavButton("Reports History", "📊", 4);
    QPushButton* settingsBtn = createNavButton("System Settings", "⚙️", 5);

    layout->addWidget(dashBtn);
    layout->addWidget(studBtn);
    layout->addWidget(faceBtn);
    layout->addWidget(attendBtn);
    layout->addWidget(reportBtn);
    layout->addWidget(settingsBtn);

    layout->addStretch();

    // Divider before logout
    QFrame* divider = new QFrame(this);
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet("background-color: #ECECEC; margin: 10px 20px;");
    layout->addWidget(divider);

    // Logout Button at the bottom
    QPushButton* logoutBtn = new QPushButton("🚪 Log Out", this);
    logoutBtn->setObjectName("SidebarBtn");
    logoutBtn->setCursor(Qt::PointingHandCursor);
    layout->addWidget(logoutBtn);

    // Route signals
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(m_btnGroup, &QButtonGroup::idClicked, this, &Sidebar::handleButtonClicked);
#else
    connect(m_btnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &Sidebar::handleButtonClicked);
#endif

    connect(logoutBtn, &QPushButton::clicked, this, &Sidebar::logoutRequested);
    
    // Select Dashboard by default
    setSelectedPage(0);
}

QPushButton* Sidebar::createNavButton(const QString& text, const QString& iconSymbol, int pageId) {
    QPushButton* btn = new QPushButton(iconSymbol + "   " + text, this);
    btn->setObjectName("SidebarBtn");
    btn->setCheckable(true);
    btn->setCursor(Qt::PointingHandCursor);

    m_btnGroup->addButton(btn, pageId);
    return btn;
}

void Sidebar::handleButtonClicked(int id) {
    emit navigationRequested(id);
}

void Sidebar::setSelectedPage(int index) {
    QAbstractButton* btn = m_btnGroup->button(index);
    if (btn) {
        btn->setChecked(true);
    }
}

} // namespace Optix::UI::Common
