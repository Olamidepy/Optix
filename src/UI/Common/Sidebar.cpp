#include "Sidebar.hpp"
#include "Application/AppContext.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <iostream>

namespace Optix::UI::Common {

static QString resolveAssetPath(const QString& relativePath) {
    QStringList searchPaths = {
        relativePath,
        "Public/" + relativePath,
        "../Public/" + relativePath,
        "../../Public/" + relativePath,
        "../../../Public/" + relativePath,
        QCoreApplication::applicationDirPath() + "/Public/" + relativePath,
        QCoreApplication::applicationDirPath() + "/../Public/" + relativePath,
        QCoreApplication::applicationDirPath() + "/../../Public/" + relativePath
    };
    for (const auto& path : searchPaths) {
        if (QFile::exists(path)) {
            return QDir::cleanPath(path);
        }
    }
    return relativePath; // Fallback
}

static QIcon getColoredIcon(const QString& path, const QColor& color) {
    QString resolved = resolveAssetPath(path);
    QPixmap pixmap(resolved);
    if (pixmap.isNull()) {
        std::cerr << "Optix UI: Failed to load icon: " << path.toStdString() << " (resolved to: " << resolved.toStdString() << ")" << std::endl;
        return QIcon();
    }
    
    QPixmap coloredPixmap(pixmap.size());
    coloredPixmap.fill(Qt::transparent);
    
    QPainter painter(&coloredPixmap);
    painter.drawPixmap(0, 0, pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(coloredPixmap.rect(), color);
    painter.end();
    
    return QIcon(coloredPixmap);
}

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
    QPushButton* dashBtn = createNavButton("Dashboard", "Home.png", 0);
    QPushButton* studBtn = createNavButton("Students", "Students.png", 1);
    QPushButton* faceBtn = createNavButton("Face Registration", "Face.png", 2);
    QPushButton* attendBtn = createNavButton("Take Attendance", "attendance.png", 3);
    QPushButton* reportBtn = createNavButton("Reports History", "Report.png", 4);
    QPushButton* settingsBtn = createNavButton("System Settings", "Setting.png", 5);

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
    QPushButton* logoutBtn = new QPushButton("  Log Out", this);
    logoutBtn->setObjectName("SidebarBtn");
    logoutBtn->setCursor(Qt::PointingHandCursor);
    QIcon logoutIcon = getColoredIcon("Logout.png", QColor("#F97316"));
    if (!logoutIcon.isNull()) {
        logoutBtn->setIcon(logoutIcon);
        logoutBtn->setIconSize(QSize(20, 20));
    }
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

QPushButton* Sidebar::createNavButton(const QString& text, const QString& iconPath, int pageId) {
    QPushButton* btn = new QPushButton("  " + text, this);
    btn->setObjectName("SidebarBtn");
    btn->setCheckable(true);
    btn->setCursor(Qt::PointingHandCursor);

    QIcon icon = getColoredIcon(iconPath, QColor("#F97316"));
    if (!icon.isNull()) {
        btn->setIcon(icon);
        btn->setIconSize(QSize(20, 20));
    }

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
